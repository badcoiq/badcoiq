﻿#include "main.h"

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
		SetDlgItemText(hWnd, IDC_EDIT_SCALE, L"1");

		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			plg->m_GUI_checkExportAnimation = IsDlgButtonChecked(hWnd, IDC_EXPANI);
			plg->m_GUI_checkOnlySkeleton = IsDlgButtonChecked(hWnd, IDC_ONLYSKEL);

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