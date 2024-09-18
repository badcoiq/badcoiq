#include "main.h"

static INT_PTR CALLBACK ExportDlgProc(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


void* ClassDescExporter::Create(BOOL loading)
{
	return new PluginExporter;
}

PluginExporter::PluginExporter()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
}

PluginExporter::~PluginExporter()
{
	for (size_t i = 0, sz = m_meshes.size(); i < sz; ++i)
	{
		delete m_meshes[i];
	}
}

int	PluginExporter::ExtCount()
{
	return 1;
}

const MCHAR* PluginExporter::Ext(int n)
{
	switch (n)
	{
	case 0:
		return L"mdl";
	}
	return L"";
}

const MCHAR* PluginExporter::LongDesc()
{
	return L"Badcoiq engine model";
}
const MCHAR* PluginExporter::ShortDesc()
{
	return L"Badсoiq MDL";
}
const MCHAR* PluginExporter::AuthorName()
{
	return L"badcoiq";
}
const MCHAR* PluginExporter::CopyrightMessage()
{
	return L"badcoiq";
}
const MCHAR* PluginExporter::OtherMessage1()
{
	return L"";
}
const MCHAR* PluginExporter::OtherMessage2()
{
	return L"";
}
unsigned int	PluginExporter::Version()
{
	return 100;
}
void PluginExporter::ShowAbout(HWND hWnd) 
{
}

	
_Material* PluginExporter::GetMaterial(uint32_t id)
{
	//printf("Get material [%u]. %zu\n", id, m_materials.size());
	if (m_materials.size())
	{
		for (size_t i = 0, sz = m_materials.size(); i < sz; ++i)
		{
			if (m_materials[i].m_header.m_nameIndex == id)
				return &m_materials[i];
		}
	}
	return &m_materials[0];
}

uint32_t PluginExporter::AddString(const char* str)
{
	uint32_t index = 0;

	auto iterator = m_stringMap.find(str);
	if (iterator == m_stringMap.end())
	{
		index = m_stringIndex;
		m_stringMap[str] = index;
		m_strings.push_back(str);

		printf("Add String: [%s][%i]\n", str, m_stringIndex);


		++m_stringIndex;
	}
	else
	{
		index = iterator->second;
	}

	return index;
}

uint32_t PluginExporter::AddString(const wchar_t* str)
{
	char cstr[0x5fff];
	WideCharToMultiByte(CP_UTF8, 0, str, -1, cstr, 0x5fff, 0, 0);

	return AddString(cstr);
}

std::string& PluginExporter::GetString(uint32_t i)
{
	return m_strings[i];
}

_Mesh* PluginExporter::CreateMesh()
{
	_Mesh* m = new _Mesh();
	m_meshes.push_back(m);
	return m;
}

void PluginExporter::Save(const MCHAR* name)
{
	printf("Save\n");

	struct _animationInfo
	{
		uint32_t m_nameIndex = 0;
		uint32_t m_framesNum = 0;

		// с какого фрейма начать копировать m_framesNum фреймов.
		// Если 1 анимация то m_framesNum это есть макс значение,
		// то есть длина анимации в редакторе. А начало анимации
		// будет 0.
		uint32_t m_startFrame = 0;
	};
	std::vector<_animationInfo> animationInfo;

	FILE* f = 0;
	char cname[0xffff];
	WideCharToMultiByte(CP_UTF8, 0, name, -1, cname, 0xffff, 0, 0);
	fopen_s(&f, cname, "wb");
	if (f)
	{
		std::vector<SubMesh*> subMeshes;
		for (auto m : m_meshes)
		{
			for (auto s : m->m_subMesh)
			{
				subMeshes.push_back(s);
			}
		}

		size_t aniNum = 0;
		bqMDLFileHeader fileHeader;
		fileHeader.m_chunkNum = 0;
		if (m_GUI_checkExportAnimation && m_hasBoneAnimationData)
		{
			aniNum = 1;
			if (aniNum == 1)
			{
				_animationInfo ai;
				ai.m_nameIndex = this->AddString("Animation");
				ai.m_framesNum = m_framesNum;
				animationInfo.push_back(ai);
			}
			else if (aniNum > 1)
			{
				// здесь будет настройка диапазонов для множества анимаций
			}

			fileHeader.m_chunkNum += aniNum;
		}

		uint32_t meshChunkNum = (uint32_t)subMeshes.size();
		size_t stringChunkNum = m_strings.size();
		size_t matChunkNum = m_materials.size();

		//m_GUI_checkExportAnimation
		if (!m_GUI_checkOnlySkeleton)
		{
			fileHeader.m_chunkNum += meshChunkNum;
			fileHeader.m_chunkNum += matChunkNum;

			// если m_GUI_checkUseCollision то
			// будет записан 1 меш для колизии.
			if (meshChunkNum && m_GUI_checkUseCollision)
			{
				fileHeader.m_chunkNum += meshChunkNum;
			}
		}

		fileHeader.m_chunkNum += stringChunkNum;



		if (m_bonesNum)
		{
			++fileHeader.m_chunkNum; // bqMDLChunkHeaderSkeleton
		}

		fileHeader.m_rotation[0] = m_rotation.x;
		fileHeader.m_rotation[1] = m_rotation.y;
		fileHeader.m_rotation[2] = m_rotation.z;
		fileHeader.m_scale = m_GUI_scale;

		printf("Write file header\n");
		fwrite(&fileHeader, 1, sizeof(fileHeader), f);

		if (!m_GUI_checkOnlySkeleton)
		{
			printf("Add meshes\n");
			for (uint32_t i = 0; i < meshChunkNum; ++i)
			{
				auto smesh = subMeshes[i];

				bqMDLChunkHeaderMesh meshChunkHeader;
				meshChunkHeader = smesh->m_chunkHeaderMesh;

				printf(" - [%u] : m_nameIndex [%u]\n", i, meshChunkHeader.m_nameIndex);
				printf(" - [%u] : m_material [%u]\n", i, meshChunkHeader.m_material);


				bqMDLChunkHeader chunkHeader;
				chunkHeader.m_chunkType = bqMDLChunkHeader::ChunkType_Mesh;
				chunkHeader.m_chunkSz = sizeof(bqMDLChunkHeader)
					+ sizeof(bqMDLChunkHeaderMesh)
					+ meshChunkHeader.m_vertBufSz
					+ meshChunkHeader.m_indBufSz;

				m_fileBuffer.Add(&chunkHeader, sizeof(chunkHeader));
				m_fileBuffer.Add(&meshChunkHeader, sizeof(meshChunkHeader));
				m_fileBuffer.Add(smesh->m_vertices, meshChunkHeader.m_vertBufSz);
				m_fileBuffer.Add(smesh->m_indices, meshChunkHeader.m_indBufSz);
			}
		}


		printf("Add strings\n");
		for (uint32_t i = 0; i < stringChunkNum; ++i)
		{
			auto& str = m_strings[i];

			bqMDLChunkHeaderString stringChunkHeader;
			stringChunkHeader.m_strSz = str.size();
			printf(" - [%u] : [%s]\n", i, str.c_str());

			bqMDLChunkHeader chunkHeader;
			chunkHeader.m_chunkType = bqMDLChunkHeader::ChunkType_String;
			chunkHeader.m_chunkSz = sizeof(bqMDLChunkHeader)
				+ sizeof(bqMDLChunkHeaderString)
				+ stringChunkHeader.m_strSz;

			m_fileBuffer.Add(&chunkHeader, sizeof(chunkHeader));
			m_fileBuffer.Add(&stringChunkHeader, sizeof(stringChunkHeader));
			m_fileBuffer.Add(str.data(), str.size());
		}

		if (m_bonesNum)
		{
			printf("Add bones\n");
			bqMDLChunkHeader chunkHeader;
			chunkHeader.m_chunkType = bqMDLChunkHeader::ChunkType_Skeleton;
			chunkHeader.m_chunkSz = sizeof(bqMDLChunkHeader)
				+ sizeof(bqMDLChunkHeaderSkeleton)
				+ (sizeof(bqMDLBoneData) * m_bonesNum);
			m_fileBuffer.Add(&chunkHeader, sizeof(chunkHeader));

			bqMDLChunkHeaderSkeleton chunkHeaderSkeleton;
			chunkHeaderSkeleton.m_boneNum = m_bonesNum;
			m_fileBuffer.Add(&chunkHeaderSkeleton, sizeof(chunkHeaderSkeleton));

			// кости нужно записать в порядке индексов от 0 до конца.
			// в map всё упорядочено по имени.
			for (int32_t i = 0; i < m_bonesNum; ++i)
			{
				SkeletonBone sBone = GetBone(i);
				//if (sBone.m_node)
				{

					bqMDLBoneData boneData;
					boneData.m_nameIndex = sBone.m_nameIndex;
					boneData.m_parent = sBone.m_parentIndex;

					/*if (sBone.m_parentIndex != -1)
					{
						SkeletonBone sBoneParent = GetBone(sBone.m_parentIndex);
						printf("Bone [%s] parent [%s]\n", GetString(sBone.m_nameIndex).c_str(),
							GetString(sBoneParent.m_nameIndex).c_str());
					}
					else
						printf("Bone [%s] NO PARENT\n", GetString(sBone.m_nameIndex).c_str());*/

						/*boneData.m_position[0] = sBone.m_position[0];
						boneData.m_position[1] = sBone.m_position[1];
						boneData.m_position[2] = sBone.m_position[2];
						boneData.m_scale[0] = sBone.m_scale[0];
						boneData.m_scale[1] = sBone.m_scale[1];
						boneData.m_scale[2] = sBone.m_scale[2];
						boneData.m_rotation[0] = sBone.m_rotation[0];
						boneData.m_rotation[1] = sBone.m_rotation[1];
						boneData.m_rotation[2] = sBone.m_rotation[2];
						boneData.m_rotation[3] = sBone.m_rotation[3];*/
					sBone.GetInitialPosition(boneData.m_position);
					sBone.GetInitialScale(boneData.m_scale);
					sBone.GetInitialRotation(boneData.m_rotation);

					m_fileBuffer.Add(&boneData, sizeof(boneData));
				}
			}
		}

		if (!m_GUI_checkOnlySkeleton)
		{
			printf("Add materials\n");
			for (uint32_t i = 0; i < matChunkNum; ++i)
			{
				_Material* mat = &m_materials[i];

				//bqMDLChunkHeaderMaterial materialChunkHeader;
				//materialChunkHeader = mat.m_header;
				//materialChunkHeader.m_nameIndex = mat.m_header.m_nameIndex;

				printf(" - [%u] : m_nameIndex [%u]\n", i, mat->m_header.m_nameIndex);
				printf(" - [%u] : m_difMap [%u]\n", i, mat->m_header.m_difMap);

				bqMDLChunkHeader chunkHeader;
				chunkHeader.m_chunkType = bqMDLChunkHeader::ChunkType_Material;
				chunkHeader.m_chunkSz = sizeof(bqMDLChunkHeader)
					+ sizeof(bqMDLChunkHeaderMaterial);

				m_fileBuffer.Add(&chunkHeader, sizeof(chunkHeader));
				m_fileBuffer.Add(&mat->m_header, sizeof(mat->m_header));
			}

			if (meshChunkNum && m_GUI_checkUseCollision)
			{
				printf("Collision\n");

				/*bqMDLChunkHeader chunkHeader;
				chunkHeader.m_chunkType = bqMDLChunkHeader::ChunkType_CollisionMesh;
				chunkHeader.m_chunkSz = sizeof(bqMDLChunkHeader)
					+ sizeof(bqMDLChunkHeaderCollisionMesh);*/


				for (uint32_t i = 0; i < meshChunkNum; ++i)
				{
					auto smesh = subMeshes[i];
				
					if (smesh->m_chunkHeaderMesh.m_indNum < 3)
						continue;

					std::map<std::string, uint32_t> vMap;
					std::vector<vec3> Vs;
					std::vector<uint32_t> Is;

					uint16_t* ind16 = (uint16_t*)smesh->m_indices;
					uint32_t* ind32 = (uint32_t*)smesh->m_indices;
					uint32_t curInd = 0;
					uint32_t tri[3] = { 0,0,0 };
					uint32_t triNum = smesh->m_chunkHeaderMesh.m_indNum / 3;
					bqMDLMeshVertex* Vtx = (bqMDLMeshVertex*)smesh->m_vertices;
					bqMDLMeshVertexSkinned* VtxS = (bqMDLMeshVertexSkinned*)smesh->m_vertices;
					vec3 triV[3];
					aabb _aabb;

					for (uint32_t ti = 0, tic = 0; ti < triNum; ++ti)
					{
						if (smesh->m_chunkHeaderMesh.m_indexType == bqMDLChunkHeaderMesh::IndexType_16bit)
						{
							tri[0] = ind16[tic];
							tri[1] = ind16[tic+1];
							tri[2] = ind16[tic+2];
						}
						else
						{
							tri[0] = ind32[tic];
							tri[1] = ind32[tic + 1];
							tri[2] = ind32[tic + 2];
						}

						tic += 3;

						if (smesh->m_chunkHeaderMesh.m_vertexType == bqMDLChunkHeaderMesh::VertexType_TriangleSkinned)
						{
							triV[0].x = VtxS[tri[0]].m_base.m_position[0];
							triV[0].y = VtxS[tri[0]].m_base.m_position[1];
							triV[0].z = VtxS[tri[0]].m_base.m_position[2];

							triV[1].x = VtxS[tri[1]].m_base.m_position[0];
							triV[1].y = VtxS[tri[1]].m_base.m_position[1];
							triV[1].z = VtxS[tri[1]].m_base.m_position[2];

							triV[2].x = VtxS[tri[2]].m_base.m_position[0];
							triV[2].y = VtxS[tri[2]].m_base.m_position[1];
							triV[2].z = VtxS[tri[2]].m_base.m_position[2];
						}
						else
						{
							triV[0].x = Vtx[tri[0]].m_position[0];
							triV[0].y = Vtx[tri[0]].m_position[1];
							triV[0].z = Vtx[tri[0]].m_position[2];

							triV[1].x = Vtx[tri[1]].m_position[0];
							triV[1].y = Vtx[tri[1]].m_position[1];
							triV[1].z = Vtx[tri[1]].m_position[2];

							triV[2].x = Vtx[tri[2]].m_position[0];
							triV[2].y = Vtx[tri[2]].m_position[1];
							triV[2].z = Vtx[tri[2]].m_position[2];
						}

						_aabb.add(triV[0]);
						_aabb.add(triV[1]);
						_aabb.add(triV[2]);

						_onCollisionMeshAddPositionInMap(triV[0], vMap, Vs, Is, curInd);
						_onCollisionMeshAddPositionInMap(triV[1], vMap, Vs, Is, curInd);
						_onCollisionMeshAddPositionInMap(triV[2], vMap, Vs, Is, curInd);
					}
					bqMDLChunkHeaderCollisionMesh meshChunkHeader;
					meshChunkHeader.m_aabb.m_aabbMax[0] = _aabb.m_max.x;
					meshChunkHeader.m_aabb.m_aabbMax[1] = _aabb.m_max.y;
					meshChunkHeader.m_aabb.m_aabbMax[2] = _aabb.m_max.z;
					meshChunkHeader.m_aabb.m_aabbMin[0] = _aabb.m_min.x;
					meshChunkHeader.m_aabb.m_aabbMin[1] = _aabb.m_min.y;
					meshChunkHeader.m_aabb.m_aabbMin[2] = _aabb.m_min.z;
					meshChunkHeader.m_aabb.m_radius = _aabb.radius();
					meshChunkHeader.m_indNum = Is.size();
					meshChunkHeader.m_vertNum = Vs.size();

					//printf("RADIUS: %f\n", meshChunkHeader.m_radius);

					uint32_t vSz = meshChunkHeader.m_vertNum * sizeof(vec3);
					uint32_t iSz = meshChunkHeader.m_indNum * sizeof(uint32_t);


					std::vector<tri_aabb> _aabbs;
					meshChunkHeader.m_numOfBVHLeaves = BuildBVH(_aabbs, Vs, Is);
					meshChunkHeader.m_numOfBVHAabbs = _aabbs.size();

										
					bqMDLChunkHeader chunkHeader;
					chunkHeader.m_chunkType = bqMDLChunkHeader::ChunkType_CollisionMesh;
					chunkHeader.m_chunkSz = sizeof(bqMDLChunkHeader)
						+ sizeof(bqMDLChunkHeaderCollisionMesh)
						+ vSz
						+ iSz
						+ (meshChunkHeader.m_numOfBVHAabbs * sizeof(bqMDLBVHAABB));

					for (size_t li = 0; li < meshChunkHeader.m_numOfBVHLeaves; ++li)
					{
						chunkHeader.m_chunkSz += sizeof(_aabbs[li].m_triNum);
						chunkHeader.m_chunkSz += _aabbs[li].m_triNum * sizeof(int32_t);
					}


					m_fileBuffer.Add(&chunkHeader, sizeof(chunkHeader));
					m_fileBuffer.Add(&meshChunkHeader, sizeof(meshChunkHeader));
					m_fileBuffer.Add(Vs.data(), vSz);
					m_fileBuffer.Add(Is.data(), iSz);

					for (size_t ai = 0; ai < meshChunkHeader.m_numOfBVHAabbs; ++ai)
					{
						auto* ab = &_aabbs[ai];

						bqMDLBVHAABB bvh_aabb;
						bvh_aabb.m_aabb.m_radius = ab->m_aabb.radius();
						bvh_aabb.m_aabb.m_aabbMin[0] = ab->m_aabb.m_min.x;
						bvh_aabb.m_aabb.m_aabbMin[1] = ab->m_aabb.m_min.y;
						bvh_aabb.m_aabb.m_aabbMin[2] = ab->m_aabb.m_min.z;
						bvh_aabb.m_aabb.m_aabbMax[0] = ab->m_aabb.m_max.x;
						bvh_aabb.m_aabb.m_aabbMax[1] = ab->m_aabb.m_max.y;
						bvh_aabb.m_aabb.m_aabbMax[2] = ab->m_aabb.m_max.z;
						bvh_aabb.m_first = ab->m_aabb_a;
						bvh_aabb.m_second = ab->m_aabb_b;

						m_fileBuffer.Add(&bvh_aabb, sizeof(bvh_aabb));
					}
					for (size_t li = 0; li < meshChunkHeader.m_numOfBVHLeaves; ++li)
					{
						auto* ab = &_aabbs[li];
						m_fileBuffer.Add(&ab->m_triNum, sizeof(ab->m_triNum));
						for (int32_t ti = 0; ti < ab->m_triNum; ++ti)
						{
							m_fileBuffer.Add(&ab->m_tris[ti], sizeof(ab->m_triNum));
						}
					}
				}
			}
		}

		if (aniNum && m_bonesNum)
		{
			printf("Add animation\n");


			// aniNum - количество анимаций.
			// Для установки количества нужно реализовать
			// GUI элементы. В данный момент анимация одна.
			// При реализации установки множества анимаций,
			// нужно будет указывать диапазоны анимаций.
			// А так-же нужно будет вычислять правильный
			// chunkHeaderAnimation.m_framesNum.

			for (uint32_t i = 0; i < aniNum; ++i)
			{
				bqMDLChunkHeader chunkHeader;
				chunkHeader.m_chunkType = bqMDLChunkHeader::ChunkType_Animation;
				chunkHeader.m_chunkSz = sizeof(bqMDLChunkHeader)
					+ sizeof(bqMDLChunkHeaderAnimation);

				bqMDLChunkHeaderAnimation chunkHeaderAnimation;
				chunkHeaderAnimation.m_nameIndex = animationInfo[i].m_nameIndex;
				chunkHeaderAnimation.m_framesNum = animationInfo[i].m_framesNum;

				m_fileBuffer.Add(&chunkHeader, sizeof(chunkHeader));
				m_fileBuffer.Add(&chunkHeaderAnimation, sizeof(chunkHeaderAnimation));

				/* Типа так
				* frame0
				*     bone0 pos sc rot
				*     bone1 pos sc rot
				*     bone2 pos sc rot
				*     bone3 pos sc rot
				* frame1
				*     bone0 pos sc rot
				*     bone1 pos sc rot
				*     bone2 pos sc rot
				*     bone3 pos sc rot
				* frame2
				*     bone0 pos sc rot
				*     bone1 pos sc rot
				*     bone2 pos sc rot
				*     bone3 pos sc rot
				*/
				for (size_t fi = animationInfo[i].m_startFrame; fi < animationInfo[i].m_framesNum; ++fi)
				{
					for (int32_t i = 0; i < m_bonesNum; ++i)
					{
						SkeletonBone sBone = GetBone(i);
						m_fileBuffer.Add(sBone.m_animationData[fi].Data(), sizeof(bqMDLAnimationData));
					}
				}

			}
		}

		fileHeader.m_compression = fileHeader.compression_fastlz;
		if (fileHeader.m_compression)
		{
			printf("Compress everything\n");
			switch (fileHeader.m_compression)
			{
			case fileHeader.compression_fastlz:
			default:
			{
				auto sizeUncompressed = m_fileBuffer.Size();
				uint8_t* output = (uint8_t*)malloc(sizeUncompressed + (sizeUncompressed / 3));
				if (output)
				{
					int compressed_size = fastlz_compress_level(
						(int)2,
						m_fileBuffer.Data(),
						sizeUncompressed,
						output);

					if ((uint32_t)compressed_size >= sizeUncompressed)
					{
					}
					else
					{
						uint8_t* output2 = (uint8_t*)realloc(output, compressed_size);
						if (output2)
						{
							output = output2;

							fileHeader.m_cmpSz = compressed_size;
							fileHeader.m_uncmpSz = sizeUncompressed;
							printf("Final write\n");
							fwrite(output, 1, compressed_size, f);

						}
					}

					free(output);
				}
			}break;
			}
		}
		else
		{
			printf("Final write\n");
			if (m_fileBuffer.Size() && m_fileBuffer.Data())
				fwrite(m_fileBuffer.Data(), 1, m_fileBuffer.Size(), f);
		}

		printf("Write header again\n");
		fseek(f, 0, SEEK_SET);
		fwrite(&fileHeader, 1, sizeof(fileHeader), f);

		fclose(f);
	}
}

	

void PluginExporter::AddBone(INode* node, Object* obj)
{
	std::string nodeName(GetAString(node->GetName()).c_str());


	// предполагается что будут добавляться
	// новые ноды, повторений не будет.
	// Но для аккуратности пусть будет поиск.
	if (m_skeleton.find(nodeName) == m_skeleton.end())
	{
		SkeletonBone b;
		b.m_index = m_bonesNum;
		b.m_node = node;
		b.m_nameIndex = AddString(nodeName.c_str());
		//printf("Add Bone: [%s][%i][%I64x] nameIndex [%u]\n", 
		//	nodeName.c_str(), m_bonesNum, (uint64_t)node,
		//	b.m_nameIndex);



		//if()

		++m_bonesNum;

		m_skeleton[nodeName] = b;
	}
}
void PluginExporter::BuildSkeleton()
{
	printf("\nFind parents...\n");

	// Нахождение всех родителей
	auto it = m_skeleton.begin();
	while (it != m_skeleton.end())
	{
		SkeletonBone& bone = (*it).second;
		/* printf("Bone: [%s][%i] ... INode[%s][%I64x]\n",
				GetString(bone.m_nameIndex).c_str(),
				bone.m_index,
				GetAString(bone.m_node->GetName()).c_str(),
				(uint64_t)bone.m_node);*/


		INode* parentNode = bone.m_node->GetParentNode();
		if (parentNode)
		{
			/*printf("Parent INode name [%s][%I64x]\n",
				GetAString(parentNode->GetName()).c_str(),
				(uint64_t)parentNode);*/

			if (parentNode != m_sceneRootNode)
			{

				SkeletonBone parentBone = GetBone(parentNode);
				if (parentBone.m_node == parentNode)
				{
					bone.m_parentIndex = parentBone.m_index;
					/*printf("Set parent: ... [%s][%i]\n",
						GetString(parentBone.m_nameIndex).c_str(),
						parentBone.m_index);*/

				}
			}
		}

		it++;
	}

	// установка позиции и т.д.
	it = m_skeleton.begin();
	while (it != m_skeleton.end())
	{
		SkeletonBone& bone = (*it).second;
		printf("Bone [%s]\n", GetAString(bone.m_node->GetName()).c_str());

		int _t = 0 * m_tickPerFrame;

		Matrix3 tm(bone.m_node->GetNodeTM(_t));
		Matrix3 ptm(bone.m_node->GetParentTM(_t));
		Control* tmc = bone.m_node->GetTMController();
		Class_ID cid = tmc->ClassID();
		if (cid == BIPBODY_CONTROL_CLASS_ID || cid == BIPED_CLASS_ID) {
			/*if (m_config.getInvertYZ()) {
				Matrix3 m = RotateXMatrix(PI / 2.0f);
				tm = tm * Inverse(m);
			}*/
		}
		else
			tm = tm * Inverse(ptm);

		Point3 pos = tm.GetTrans();

		AngAxis aa(tm);
		Quat q(tm);

		Control* sC = tmc->GetScaleController();
		ScaleValue sc;
		if (sC)
			sC->GetValue(m_timeValue, &sc);

		bone.SetInitialPosition(&pos.x);
		bone.SetInitialScale(&sc.s.x);
		bone.SetInitialRotation(&q.x);

		it++;
	}

	it = m_skeleton.begin();
	while (it != m_skeleton.end())
	{
		for (int i = 0; i < m_framesNum; ++i)
		{
			int _t = i * m_tickPerFrame;

			SkeletonBone& bone = (*it).second;

			Matrix3 tm(bone.m_node->GetNodeTM(_t));
			Matrix3 ptm(bone.m_node->GetParentTM(_t));
			Control* tmc = bone.m_node->GetTMController();
			tm = tm * Inverse(ptm);


			Point3 pos = tm.GetTrans();
			AngAxis aa(tm);



			Quat q(tm);
			Control* sC = tmc->GetScaleController();
			ScaleValue sc;
			if (sC)
				sC->GetValue(_t, &sc);

			SkeletonBoneTransformation bnTr;
			bnTr.SetPosition(&pos.x);
			bnTr.SetScale(&sc.s.x);
			bnTr.SetRotation(&q.x);

			bone.m_animationData.push_back(bnTr);
			m_hasBoneAnimationData = true;
		}
		it++;
	}

}

PluginExporter::SkeletonBone PluginExporter::GetBone(const char* name)
{
	SkeletonBone bone;

	if (name)
	{
		auto it = m_skeleton.find(name);
		if (it != m_skeleton.end())
		{
			bone = (*it).second;
		}
	}

	return bone;
}
PluginExporter::SkeletonBone PluginExporter::GetBone(INode* node)
{
	/* printf("GetBoneIBone [%s][%llu]\n",
			GetAString(node->GetName()).c_str(),
			(uint64_t)node);*/

	SkeletonBone bone;
	if (node)
	{
		auto it = m_skeleton.begin();
		while (it != m_skeleton.end())
		{
			SkeletonBone& _b = (*it).second;

			/* printf("\t- - - [%s][%llu]  -  -  - [%s][%llu]\n",
					GetAString(node->GetName()).c_str(),
					(uint64_t)node,
					GetAString(_b.m_node->GetName()).c_str(),
					(uint64_t)_b.m_node);*/

			if ((uint64_t)node == (uint64_t)_b.m_node)
			{
				// printf("\t- - - FOUND\n");
				return _b;
			}
			it++;
		}
	}
	return bone;
}
PluginExporter::SkeletonBone PluginExporter::GetBone(int index)
{
	SkeletonBone bone;
	if (index >= 0)
	{
		auto it = m_skeleton.begin();
		while (it != m_skeleton.end())
		{
			if ((*it).second.m_index == index)
			{
				return (*it).second;
			}
			it++;
		}
	}
	return bone;
}


std::string PluginExporter::GetAString(const wchar_t* str)
{
	MaxSDK::Util::MaxString mstr;
	mstr.AllocBuffer(0xfff);
	mstr.SetUTF16(str);

	size_t len = 0;
	auto astr = mstr.ToUTF8(&len);

	std::string s;

	if (len)
		s.append(astr.data());

	return s;
}


void PluginExporter::MtlGetBitmap(std::string& str, Texmap* tMap)
{
	BitmapTex* bitmap = dynamic_cast<BitmapTex*>(tMap);
	if (bitmap)
	{
		MSTR path = bitmap->GetMap().GetFileName();
		if (path)
		{
			std::filesystem::path p = GetAString(path);
			str = p.filename().string();
			// wprintf(L"File path [%s]\n", path);
		}
	}
}

void PluginExporter::AddMtl(Mtl* mtl, _Mesh* _mesh)
{
	if (mtl)
	{
		//Base Color Map
		std::string phsMtl_mapName_base = "Base Color Map";
		//Bitmaptexture
		std::string mapName_bitmaptexture = "Bitmaptexture";

		printf("AddMtl [%u][%u]\n", mtl->ClassID().PartA(), mtl->ClassID().PartB());
		//	printf("NumSubMtls %i\n", mtl->NumSubMtls());

		printf("MTL Name: [%s]\n", GetAString(mtl->GetName()).c_str());

		if (mtl->ClassID() == MULTI_MATERIAL_CLASS_ID)
		{
			_mesh->m_multiMat = true;

			int subMtlsNum = mtl->NumSubMtls();
			if (subMtlsNum)
			{
				//	printf("MULTI_MATERIAL_CLASS_ID\n");

				for (int i2 = 0; i2 < subMtlsNum; ++i2)
				{
					AddMtl(mtl->GetSubMtl(i2), _mesh);
				}
			}
		}
		else
		{
			Color ambient = mtl->GetAmbient();
			Color diffuse = mtl->GetDiffuse();
			float selfIllum = mtl->GetSelfIllum();
			Color selIllumColor = mtl->GetSelfIllumColor();
			float shininess = mtl->GetShininess();
			float shininessStr = mtl->GetShinStr();
			Color specular = mtl->GetSpecular();

			std::string difMap;
			std::string specMap;
			std::string normMap;
			std::string rougMap;

			if (mtl->ClassID() == PHYSICALMATERIAL_CLASS_ID)
			{
				//	printf("MTL ClassID: [PHYSICALMATERIAL_CLASS_ID]\n");

				auto numSubTexMaxs = mtl->NumSubTexmaps();
				//printf("TexMap num [%i]\n", numSubTexMaxs);
				if (numSubTexMaxs)
				{
					for (int sti = 0; sti < numSubTexMaxs; ++sti)
					{
						std::string mapName = GetAString(mtl->GetSubTexmapSlotName(sti, false));
						//	printf("Texmap: %s\n", mapName.c_str());

						if (mapName == phsMtl_mapName_base)
						{

							Texmap* tMap = mtl->GetSubTexmap(sti);
							if (tMap)
							{

								//wprintf(L"Texmap [%s]\n", tMap->);

								/*Class_ID cid = tMap->ClassID();
								printf("cid: [%u] [%u]\n", cid.PartA(), cid.PartB());*/
								if (tMap->ClassID().PartA() == BMTEX_CLASS_ID)
								{
									//	printf("Bitmap\n");

									this->MtlGetBitmap(difMap, tMap);
									printf("difMap [%s]\n", difMap.c_str());
								}
								//mapName = GetAString(tMap->GetName());
								//printf("Sub Texmap: %s\n", mapName.c_str());
							}
						}
					}
				}
			}

			int mtlNameIndex = AddString(mtl->GetName());
			_Material newMat;
			newMat.m_header.m_nameIndex = mtlNameIndex;
			printf("-   : [%u]\n", newMat.m_header.m_nameIndex);

			newMat.m_header.m_ambient[0] = ambient.r;
			newMat.m_header.m_ambient[1] = ambient.g;
			newMat.m_header.m_ambient[2] = ambient.b;
			newMat.m_header.m_diffuse[0] = diffuse.r;
			newMat.m_header.m_diffuse[1] = diffuse.g;
			newMat.m_header.m_diffuse[2] = diffuse.b;
			newMat.m_header.m_specular[0] = specular.r;
			newMat.m_header.m_specular[1] = specular.g;
			newMat.m_header.m_specular[2] = specular.b;

			if (difMap.size()) newMat.m_header.m_difMap = (int32_t)AddString(difMap.c_str());
			if (specMap.size()) newMat.m_header.m_specMap = (int32_t)AddString(specMap.c_str());
			if (normMap.size()) newMat.m_header.m_normMap = (int32_t)AddString(normMap.c_str());
			if (rougMap.size()) newMat.m_header.m_rougMap = (int32_t)AddString(rougMap.c_str());

			m_materials.push_back(newMat);
			_mesh->m_materials.push_back(mtlNameIndex);
		}
	}

	printf("\n");
}

// надо добавить определённую информацию чтобы потом
// спокойно создать sub meshes.
// информация не касающаяся subMesh
void PluginExporter::AddMesh(INode* node, Object* obj)
{
	printf("Add Mesh\n");
	printf("-\tName: %s\n", GetAString(node->GetName()).c_str());

	bqMDLChunkHeaderMesh meshHeader;
	meshHeader.m_nameIndex = AddString(node->GetName());

	TriObject* triObj = dynamic_cast<TriObject*>(obj);
	if (triObj)
	{
		printf("Face Num %i\n", triObj->mesh.getNumFaces());

		Modifier* skinModifier = 0;
		ISkin* skin = 0;
		ISkinContextData* skinData = 0;
		Object* pObj = node->GetObjectRef();
		while (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
		{
			IDerivedObject* pDerObj = (IDerivedObject*)(pObj);
			int Idx = 0;
			while (Idx < pDerObj->NumModifiers())
			{
				// Get the modifier. 
				Modifier* mod = pDerObj->GetModifier(Idx);
				if (mod->ClassID() == SKIN_CLASSID)
				{
					skinModifier = mod;
					break;
				}
				Idx++;
			}
			pObj = pDerObj->GetObjRef();
		}
		if (skinModifier)
		{
			skin = (ISkin*)skinModifier->GetInterface(I_SKIN);
			if (skin)
				skinData = skin->GetContextInterface(node);
		}
		//skinData = 0;

		Mesh* mesh = &triObj->mesh;
		int faceNum = mesh->getNumFaces();

		if (faceNum)
		{

			_Mesh* newM = new _Mesh();
			newM->m_triObj = triObj;
			newM->m_node = node;
			if (skinData)
			{
				newM->m_skin = skin;
				newM->m_skinData = skinData;
			}
			m_meshes.push_back(newM);

			if (node->GetMtl())
				AddMtl(node->GetMtl(), newM);
		}
	}
}

Point3 PluginExporter::GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
{
	Face* f = &mesh->faces[faceNo];
	DWORD smGroup = f->smGroup;
	int numNormals = 0;
	Point3 vertexNormal;

	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL) {
		vertexNormal = rv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ((numNormals = rv->rFlags & NORCT_MASK) != 0 && smGroup) {
		// If there is only one vertex is found in the rn member.
		if (numNormals == 1) {
			vertexNormal = rv->rn.getNormal();
		}
		else {
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i2 = 0; i2 < numNormals; i2++) {
				if (rv->ern[i2].getSmGroup() & smGroup) {
					vertexNormal = rv->ern[i2].getNormal();
				}
			}
		}
	}
	else {
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = mesh->getFaceNormal(faceNo);
	}

	return vertexNormal;
}

void PluginExporter::BuildMeshes()
{
	printf("BuildMeshes\n");
	printf("Num of meshes %zu\n", m_meshes.size());

	// для модели без skin
	// возможно другие значения так-же должны быть с минусом
	// пришлось добавить минус, почему-то крутит в 
	// противоположную сторону 
	Quat qRot;
	qRot.SetEuler(-m_rotation.x, m_rotation.y, m_rotation.z);
	Matrix3 mRot;
	qRot.MakeMatrix(mRot);
	Matrix3 mSc;
	mSc.SetScale(Point3(m_GUI_scale, m_GUI_scale, m_GUI_scale));

	// надо построить sub mesh
	// количество sub mesh зависит от количества материалов
	for (size_t i = 0, sz = m_meshes.size(); i < sz; ++i)
	{
		printf("Mesh[%zu]\n", i);
		_Mesh* newMesh = m_meshes[i];
		Mesh* maxMesh = &newMesh->m_triObj->mesh;
		printf("Node MtlNum[%u]\n", newMesh->m_node->NumMtls());
		printf("Node Sub Material Num[%u]\n", newMesh->m_node->GetMtl()->NumSubMtls());

		maxMesh->checkNormals(TRUE);
		auto faces = maxMesh->getFacePtr(0);
		int faceNum = maxMesh->getNumFaces();
		auto rVerts = maxMesh->getRVertPtr(0);
		auto tVerts = maxMesh->getTVertPtr(0);
		int vertInds[3] = { 0, 2, 1 };

		printf("Init submeshes\n");
		for (int fi = 0; fi < faceNum; ++fi)
		{
			Face* face = &faces[fi];

			// mtlID это ID относительно INode.
			// Разные INode с разными материалами
			// вернут 0. Имея mtlID надо получить
			// ID относительно всех материалов.
			MtlID mtlID = 0;
			if (newMesh->m_multiMat)
			{
				// getMatID только для мультиматериала
				mtlID = face->getMatID();
				//printf("mtlID %u\n", mtlID);
			}
			//	printf("%i mtlID %u\n", __LINE__, mtlID);
			uint32_t matIndex = newMesh->GetMaterial(mtlID);
			//	printf("%i matIndex %i\n", __LINE__, matIndex);
			_Material* material = GetMaterial(matIndex);

			// инициализирую newMesh->m_submeshData
			SubMesh* sd = newMesh->GetSubMesh(material);
			//и подсчёт количества треугольников
			++sd->m_faceNum;
		}

		printf("Build submeshes\n");
		for (size_t si = 0, sisz = newMesh->m_subMesh.size();
			si < sisz; ++si)
		{
			SubMesh* subMesh = newMesh->m_subMesh[si];
			//  printf("sd %zu facenum %u\n", si, sd->m_faceNum);

				// зная количество face можно выделить память
				// для буферов и заполнить всю информацию
			bqMDLChunkHeaderMesh meshHeader;
			meshHeader.m_vertNum = subMesh->m_faceNum * 3;
			meshHeader.m_indexType = (meshHeader.m_vertNum < 0xffff)
				? bqMDLChunkHeaderMesh::IndexType_16bit
				: bqMDLChunkHeaderMesh::IndexType_32bit;
			meshHeader.m_indNum = meshHeader.m_vertNum;
			meshHeader.m_vertexType = bqMDLChunkHeaderMesh::VertexType_Triangle;
			if (newMesh->m_skinData)
				meshHeader.m_vertexType = bqMDLChunkHeaderMesh::VertexType_TriangleSkinned;
			subMesh->m_stride = sizeof(bqMDLMeshVertex);
			if (meshHeader.m_vertexType == bqMDLChunkHeaderMesh::VertexType_TriangleSkinned)
				subMesh->m_stride = sizeof(bqMDLMeshVertexSkinned);
			meshHeader.m_vertBufSz = meshHeader.m_vertNum * subMesh->m_stride;
			if (meshHeader.m_indexType == bqMDLChunkHeaderMesh::IndexType_16bit)
				meshHeader.m_indBufSz = meshHeader.m_indNum * 2;
			else
				meshHeader.m_indBufSz = meshHeader.m_indNum * 4;
			meshHeader.m_material = subMesh->m_materialIndex;

			if (!subMesh->Allocate(meshHeader))
				return;

			uint16_t* in16 = (uint16_t*)subMesh->m_indices;
			uint32_t* in32 = (uint32_t*)subMesh->m_indices;
			// количество индексов будет равно количеству вершин
			// на каждую вершину свой индекс
			// значит индексы будут просто идти попорядку 0 1 2 3 4 5...
			for (uint32_t i2 = 0; i2 < meshHeader.m_indNum; ++i2)
			{
				if (meshHeader.m_indexType == bqMDLChunkHeaderMesh::IndexType_16bit)
					*in16 = i2;
				else
					*in32 = i2;

				++in16;
				++in32;
			}
			//	printf("%i\n", __LINE__);

			auto maxVerts = maxMesh->getVertPtr(0);
			int numTVx = maxMesh->getNumTVerts();

			uint8_t* vert8ptr = subMesh->m_vertices;
			uint8_t* vptr = vert8ptr;
			bqMDLMeshVertex* meshVertex = (bqMDLMeshVertex*)vptr;
			for (int fi = 0; fi < faceNum; ++fi)
			{
				Face* face = &faces[fi];

				MtlID mtlID = 0;
				if (newMesh->m_multiMat)
					mtlID = face->getMatID();
				uint32_t matIndex = newMesh->GetMaterial(mtlID);
				_Material* material = GetMaterial(matIndex);

				if (subMesh->m_materialIndex == material->m_header.m_nameIndex)
				{
					int vx1 = 0;
					int vx2 = 1;
					int vx3 = 2;
					for (int ii = 0; ii < 3; ++ii)
					{
						auto faceVertIndex = face->v[vertInds[ii]];
						Point3 pos = maxVerts[faceVertIndex];
						Point3 vn = GetVertexNormal(maxMesh, fi, maxMesh->getRVertPtr(faceVertIndex));

						if (meshHeader.m_vertexType == bqMDLChunkHeaderMesh::VertexType_Triangle)
						{
							pos = pos * mRot;
							pos = pos * mSc;

							vn = vn * mRot;
						}

						meshVertex->m_position[vx1] = pos.x;
						meshVertex->m_position[vx2] = pos.y;
						meshVertex->m_position[vx3] = pos.z;
						meshVertex->m_normal[vx1] = vn.x;
						meshVertex->m_normal[vx2] = vn.y;
						meshVertex->m_normal[vx3] = vn.z;
						subMesh->AabbAdd(pos);

						if (numTVx)
						{
							auto tci1 = maxMesh->tvFace[fi].t[vertInds[ii]];
							meshVertex->m_uv[0] = tVerts[tci1].x;
							meshVertex->m_uv[1] = 1.f - tVerts[tci1].y;
						}

						if (newMesh->m_skinData)
						{
							int boneNum = newMesh->m_skin->GetNumBones();
							std::vector<INode*> _bones;
							for (int bi = 0; bi < boneNum; ++bi)
							{
								_bones.push_back(newMesh->m_skin->GetBone(bi));
							}

							bqMDLMeshVertexSkinned* meshVertexSkinned = (bqMDLMeshVertexSkinned*)vptr;
							int nbones = newMesh->m_skinData->GetNumAssignedBones(faceVertIndex);
							for (int bi = 0; bi < nbones; ++bi)
							{
								meshVertexSkinned->m_weights[bi] = newMesh->m_skinData->GetBoneWeight(faceVertIndex, bi);
								int boneIndex = newMesh->m_skinData->GetAssignedBone(faceVertIndex, bi);
								INode* _b = _bones[boneIndex];
								SkeletonBone skeletonBone = GetBone(GetAString(_b->GetName()).c_str());
								meshVertexSkinned->m_boneInds[bi] = skeletonBone.m_index;
							}
						}

						vptr += subMesh->m_stride;
						meshVertex = (bqMDLMeshVertex*)vptr;
					}
				}
			}
		}
	}


	printf("End of BuildMeshes\n");
}

int PluginExporter::DoExport(const MCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts, DWORD options)
{
	m_ei = ei;
	m_ip = ip;

	Interval range = m_ip->GetAnimRange();
	m_tickPerFrame = GetTicksPerFrame();
	m_framesNum = range.Duration() / m_tickPerFrame;

	if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1),
		ip->GetMAXHWnd(), ExportDlgProc, (LPARAM)this))
	{
		return 1;
	}

	IFrameTagManager* ftm = dynamic_cast<IFrameTagManager*>(GetCOREInterface(FRAMETAGMANAGER_INTERFACE));
	if (ftm)
	{
		//	ftm->
	}

	m_timeValue = m_ip->GetAnimRange().Start();
	m_sceneRootNode = m_ip->GetRootNode();

	// Сначала надо получить всё необходимое.
	// Потом записывать в файл.


	MyITreeEnumProc nodeEnumerator(this, m_ip);
	m_ei->theScene->EnumTree(&nodeEnumerator);

	BuildSkeleton();
	BuildMeshes();

	Save(name);
	return 1;
}

std::string g_positionString;
void PluginExporter::_onCollisionMeshAddPositionInMap(
	const vec3& pos, 
	std::map<std::string, uint32_t>& vMap,
	std::vector<vec3>& Vs,
	std::vector<uint32_t>& Is,
	uint32_t& curInd)
{
	g_positionString.clear();
	char charBuf[500];
	sprintf_s(charBuf, 500, "%f %f %f", pos.x, pos.y, pos.z);
	g_positionString.assign(charBuf);

	uint32_t indexThis = curInd;

	auto it = vMap.find(g_positionString);
	if (it == vMap.end())
	{
		vMap[g_positionString] = curInd;
		++curInd;
		Vs.push_back(pos);
	}
	else
	{
		indexThis = vMap[g_positionString];
	}

	Is.push_back(indexThis);
}

tri_aabb* BVHFindNearestAabb(tri_aabb* node, std::vector<tri_aabb>& aabbs, int prevLevel)
{
	tri_aabb* nearest = 0;

	float distance = 9999999.f;

	for (size_t ti = 0, sz = aabbs.size(); ti < sz; ++ti)
	{
		auto* curr = &aabbs[ti];
		if ((curr->m_flags & tri_aabb::flag_added) == 0)
		{
			if (curr != node)
			{
				if (curr->m_level == prevLevel)
				{
					float d = curr->m_center.distance(node->m_center);
					if (d < distance)
					{
						distance = d;
						nearest = curr;
					}
				}
			}
		}
	}


	return nearest;
}
void BuildBVH(std::vector<tri_aabb>& aabbs, int level)
{
	int numOfAdded = 0;

	int prevLevel = level - 1;
	for (size_t ti = 0, sz = aabbs.size(); ti < sz; ++ti)
	{
		auto* curr = &aabbs[ti];

		// беру те ноды, уровень которых уже установлен, он prevLevel
		// текущий уровень это level, он будет передаваться новым нодам
		if (curr->m_level == prevLevel)
		{
			// только ноды которые не добавлены
			if ((curr->m_flags & tri_aabb::flag_added) == 0)
			{
				// помечу что теперь нода добавлена
				curr->m_flags |= tri_aabb::flag_added;

				// нужно найти ближайшую ноду
				auto* nearest = BVHFindNearestAabb(curr, aabbs, prevLevel);

				// новая нода содержит или 1 или 2 tri_aabb
				tri_aabb brandNewNode;
				brandNewNode.m_aabb_a = curr->m_indexInAabbs;
				brandNewNode.m_aabb.add(curr->m_aabb);

				if (nearest)
				{
					nearest->m_flags |= tri_aabb::flag_added;
					brandNewNode.m_aabb_b = nearest->m_indexInAabbs;
					brandNewNode.m_aabb.add(nearest->m_aabb);
				}
				brandNewNode.m_center = brandNewNode.m_aabb.center();
				brandNewNode.m_level = level;
				brandNewNode.m_indexInAabbs = aabbs.size();
				aabbs.push_back(brandNewNode);
				++numOfAdded;
			}
		}
	}

	if (numOfAdded > 1)
	{
		for (size_t ti = 0, sz = aabbs.size(); ti < sz; ++ti)
		{
			auto* curr = &aabbs[ti];
			if (curr->m_level == level)
			{
				BuildBVH(aabbs, level + 1);
			}
		}
	}
}
uint32_t PluginExporter::BuildBVH(
	std::vector<tri_aabb>& aabbs, 
	std::vector<vec3>& Vs, 
	std::vector<uint32_t>& Is)
{
	uint32_t triNum = Is.size() / 3;
	std::vector<tri_aabb> tri_aabbs;
	tri_aabbs.reserve(triNum);

	// получаю аабб на каждый треугольник
	for (uint32_t ti = 0, tic = 0; ti < triNum; ++ti)
	{
		uint32_t ind1 = Is[tic];
		uint32_t ind2 = Is[tic+1];
		uint32_t ind3 = Is[tic+2];

		vec3& v1 = Vs[ind1];
		vec3& v2 = Vs[ind2];
		vec3& v3 = Vs[ind3];

		tri_aabb newTriAabb;
		newTriAabb.m_tris[0] = ti;
		newTriAabb.m_triNum = 1;
		newTriAabb.m_aabb.add(v1);
		newTriAabb.m_aabb.add(v2);
		newTriAabb.m_aabb.add(v3);
		newTriAabb.m_center = newTriAabb.m_aabb.center();

		tri_aabbs.push_back(newTriAabb);
		tic += 3;
	}

	uint32_t groupNum = 0;
	// Группирую
	for (uint32_t ti = 0; ti < triNum; ++ti)
	{

		// беру текущий
		//  *
		// [1][2][3][4][5][6][7][8][9]
		// 
		// измеряю расстояние.
		// [1] [2]  [3]  [4]  [5]
		// [0] [1.1][0.4][0.2][0.8]
		// 
		// сортирую массив.
		// [1][4][3][5][2]
		// 
		// добавляю самые близкие.
		// например 4 и 3 если добавляются только 2
		// 
		// потом надо будет сбросить ti до 0 и пройтись
		// по массиву заново. скипать обработанные aabb
		// надо используя флаг tri_aabb::flag_added
		//

		tri_aabb& currAabb = tri_aabbs[ti];
		currAabb.m_distance = 0.f;

		if ((currAabb.m_flags & tri_aabb::flag_added) == 0)
		{
			currAabb.m_flags |= tri_aabb::flag_added;
			currAabb.m_flags |= tri_aabb::flag_main;
			++groupNum;
		
			// удаляю флаг flag_distanceCalculated
			// надо сбросить так как новая итерация
			for (uint32_t ti2 = ti + 1; ti2 < triNum; ++ti2)
			{
				tri_aabbs[ti2].remove_flag(tri_aabb::flag_distanceCalculated);
			}

			bool needSort = false;
			// Измеряю расстояние
			for (uint32_t ti2 = ti+1; ti2 < triNum; ++ti2)
			{
				tri_aabb& otherAabb = tri_aabbs[ti2];
			
				if ((otherAabb.m_flags & tri_aabb::flag_distanceCalculated) == 0)
				{
					if ((otherAabb.m_flags & tri_aabb::flag_added) == 0)
					{
						otherAabb.m_distance = currAabb.m_center.distance(otherAabb.m_center);
					//	printf("TI2[%u] dist[%f]\n", ti2, otherAabb.m_distance);
						otherAabb.m_flags |= tri_aabb::flag_distanceCalculated;

						needSort = true;
					}
				}
			}

			// сортировка
			if (needSort)
			{
				std::sort(tri_aabbs.begin(), tri_aabbs.end(),
					[](const tri_aabb& a, const tri_aabb& b)
					{
						return a.m_distance < b.m_distance;
					});
			}
			
			// добавление близких
			for (uint32_t ti2 = ti + 1; ti2 < triNum; ++ti2)
			{
				tri_aabb& otherAabb = tri_aabbs[ti2];
				// если ранее не был добавлен
				if ((otherAabb.m_flags & tri_aabb::flag_added) == 0)
				{
					if (currAabb.m_triNum == TRI_AABB_MAXTRIS)
						break;

					otherAabb.m_flags |= tri_aabb::flag_added;
			//		printf("ADD TI2[%u] dist[%f]\n", ti2, otherAabb.m_distance);

					// при добавлении надо добавить аабб
					// увеличив currAabb.m_aabb
					// и надо передать индекс треугольника
					currAabb.m_aabb.add(otherAabb.m_aabb);
					currAabb.m_center = currAabb.m_aabb.center();
					currAabb.m_tris[currAabb.m_triNum] = otherAabb.m_tris[0];
					++currAabb.m_triNum;
				}
			}

			// так как была сортировка, смысла нет продолжать проход
			// по массиву. нужно запустить проход заново
			ti = 0xFFFFFFFF; // потом будет ++ti и оно станет нулём
		}
	}
	printf("groupNum %u\n", groupNum);

	uint32_t leaves = 0;
	for (uint32_t ti = 0; ti < tri_aabbs.size(); ++ti)
	{
		tri_aabb& currAabb = tri_aabbs[ti];
		if (currAabb.m_flags & tri_aabb::flag_main)
		{
			++leaves;
			currAabb.clear_flags();
			currAabb.m_indexInAabbs = aabbs.size();
			aabbs.push_back(currAabb);
		}
	}

	::BuildBVH(aabbs, 1);

	tri_aabb* root = &aabbs[aabbs.size() - 1];
	if (root)
	{
		for (uint32_t li = 0, sz = aabbs.size(); li < sz; ++li)
		{
			tri_aabb* currAabb = &aabbs[li];
			printf("NODE[%u] a[%u] b[%u] triNum[%i]\n", li,
				currAabb->m_aabb_a,
				currAabb->m_aabb_b,
				currAabb->m_triNum);
		}
	}

	return leaves;
}

BOOL PluginExporter::SupportsOptions(int ext, DWORD options) 
{
	UNUSED_PARAM(ext); 
	UNUSED_PARAM(options);
	return FALSE; 
}

static INT_PTR CALLBACK ExportDlgProc(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PluginExporter* plg = DLGetWindowLongPtr<PluginExporter*>(hWnd);
	switch (msg) {
	case WM_INITDIALOG:
		plg = (PluginExporter*)lParam;
		DLSetWindowLongPtr(hWnd, lParam);
		CenterWindow(hWnd, GetParent(hWnd));
		CheckDlgButton(hWnd, IDC_EXPANI, plg->m_GUI_checkExportAnimation);
		CheckDlgButton(hWnd, IDC_ONLYSKEL, plg->m_GUI_checkOnlySkeleton);
		CheckDlgButton(hWnd, IDC_CHECK_USE_COLLISION, plg->m_GUI_checkUseCollision);
		SetDlgItemText(hWnd, IDC_EDIT_SCALE, L"1");

		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			plg->m_GUI_checkExportAnimation = IsDlgButtonChecked(hWnd, IDC_EXPANI);
			plg->m_GUI_checkOnlySkeleton = IsDlgButtonChecked(hWnd, IDC_ONLYSKEL);
			plg->m_GUI_checkUseCollision = IsDlgButtonChecked(hWnd, IDC_CHECK_USE_COLLISION);

			char dlgitemTextBuf[1000];
			GetDlgItemTextA(hWnd, IDC_EDIT_SCALE, dlgitemTextBuf, 1000);
			GUI_GetNumbers(dlgitemTextBuf, "%f", &plg->m_GUI_scale);
			if (plg->m_GUI_scale <= 0.f)
				plg->m_GUI_scale = 1.f;
			if (plg->m_GUI_scale > 999999.f)
				plg->m_GUI_scale = 999999.f;

			EndDialog(hWnd, 1);
			break;
		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

