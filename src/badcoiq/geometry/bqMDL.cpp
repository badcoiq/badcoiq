/*
BSD 2-Clause License

Copyright (c) 2024, badcoiq
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "badcoiq.h"

#ifdef BQ_WITH_MESH

#include "badcoiq/geometry/bqMDL.h"
#include "badcoiq/common/bqFileBuffer.h"
#include "badcoiq/gs/bqGS.h"

#include <vector>


bqMDL::bqMDL()
{
}

bqMDL::~bqMDL()
{
	Unload();
}

uint8_t* bqMDL::_decompress(const bqMDLFileHeader& fileHeader,
	bqFileBuffer& fileBuffer,
	uint32_t* szOut,
	bqCompressorType ct)
{
	bqCompressionInfo cInfo;
	cInfo.m_compressorType = ct;
	
	cInfo.m_sizeCompressed = fileHeader.m_cmpSz;
	cInfo.m_dataCompressed = (uint8_t*)malloc(cInfo.m_sizeCompressed);
	if (cInfo.m_dataCompressed)
	{
		fileBuffer.Read(cInfo.m_dataCompressed, cInfo.m_sizeCompressed);
	}
	else
	{
		return 0;
	}

	cInfo.m_sizeUncompressed = fileHeader.m_uncmpSz;
	cInfo.m_dataUncompressed = (uint8_t*)malloc(cInfo.m_sizeUncompressed);
	if (cInfo.m_dataUncompressed)
	{
		if (bqArchiveSystem::Decompress(&cInfo))
		{
			free(cInfo.m_dataCompressed);

			*szOut = cInfo.m_sizeUncompressed;
			return cInfo.m_dataUncompressed;
		}
		else
		{
			free(cInfo.m_dataUncompressed);
		}
	}

	free(cInfo.m_dataCompressed);
	return 0;
}

bool bqMDL::Load(const char* fn, const char* textureDir, bqGS* gs, bool free_bqMesh)
{
	Unload();

	BQ_ASSERT_ST(fn);
	BQ_ASSERT_ST(gs);
	if (fn && gs)
	{
		bqArray<bqMDLChunkHeaderMaterial> materials;
		auto l_getMaterialHeader = [&](uint32_t nameIndex) -> bqMDLChunkHeaderMaterial*
		{
			for (size_t i = 0; i < materials.m_size; ++i)
			{
				if (materials.m_data[i].m_nameIndex == nameIndex)
					return &materials.m_data[i];
			}

			return &materials.m_data[0];
		};

		bqMDLFileHeader fileHeader;
		uint32_t fileSz = 0;
		uint8_t* ptr = bqFramework::CreateFileBuffer(fn, &fileSz, false);
		if (ptr)
		{
			materials.reserve(100);

			// для автоматического уничтожения ptr
			BQ_PTR_D(uint8_t, ptrr, ptr);

			bqFileBuffer file(ptr, fileSz);

			file.Read(&fileHeader, sizeof(fileHeader));

			if (fileHeader.m_bmld != 1818520930)
			{
				bqLog::PrintError("bqMDL: bad magic number\n");
				return false;
			}

			if (!fileHeader.m_chunkNum)
			{
				bqLog::PrintError("bqMDL: 0 chunks\n");
				return false;
			}

			if (fileHeader.m_compression)
			{
				switch (fileHeader.m_compression)
				{
				case fileHeader.compression_fastlz:
				default:
					uint32_t dDataSz = 0;
					uint8_t* dData = _decompress(fileHeader,
						file, &dDataSz, bqCompressorType::fastlz);

					if (dData)
					{
						m_compressedData = dData;
						file = bqFileBuffer(dData, dDataSz);
					}
					break;
				}
			}

			bqStringA stra;
			bqArray<char8_t> straArray;

			/*bqMDLChunkHeaderAnimation chunkHeaderAnimation;
			struct _animation
			{
				bqMDLChunkHeaderAnimation chunkHeaderAnimation;
				
				struct _frame
				{
					std::vector<bqMDLAnimationData> m_boneTransformations;
				};
				
				std::vector<_frame> m_frames;
			};
			std::vector<_animation> animations;*/

			for (uint32_t i = 0; i < fileHeader.m_chunkNum; ++i)
			{
				bqMDLChunkHeader chunkHeader;
				file.Read(&chunkHeader, sizeof(chunkHeader));

				switch (chunkHeader.m_chunkType)
				{
				case bqMDLChunkHeader::ChunkType_Mesh: //bqMDLChunkHeaderMesh
				{
					bqMDLChunkHeaderMesh chunkHeaderMesh;
					file.Read(&chunkHeaderMesh, sizeof(chunkHeaderMesh));

					if (chunkHeaderMesh.m_indexType > 1)
					{
						bqLog::PrintError("bqMDL: bad index type\n");
						return false;
					}

					if (chunkHeaderMesh.m_vertexType > 1)
					{
						bqLog::PrintError("bqMDL: bad vertex type\n");
						return false;
					}

					if (!chunkHeaderMesh.m_vertNum)
					{
						bqLog::PrintError("bqMDL: bad vertex num\n");
						return false;
					}

					if (!chunkHeaderMesh.m_indNum)
					{
						bqLog::PrintError("bqMDL: bad index num\n");
						return false;
					}

					if (!chunkHeaderMesh.m_vertBufSz)
					{
						bqLog::PrintError("bqMDL: bad vertex buffer size\n");
						return false;
					}

					if (!chunkHeaderMesh.m_indBufSz)
					{
						bqLog::PrintError("bqMDL: bad index buffer size\n");
						return false;
					}


					m_aabb.Add(chunkHeaderMesh.m_aabb.m_aabb);
					m_radius = m_aabb.Radius();

					bqMesh* newM = new bqMesh;
					newM->Allocate(chunkHeaderMesh.m_vertNum,
						chunkHeaderMesh.m_indNum,
						(chunkHeaderMesh.m_vertexType == bqMDLChunkHeaderMesh::VertexType_Triangle) 
						? false : true);					

					file.Read(newM->GetVBuffer(), chunkHeaderMesh.m_vertBufSz);
					file.Read(newM->GetIBuffer(), chunkHeaderMesh.m_indBufSz);

					//newM->GenerateNormals(true);

					_mesh m;
					if (chunkHeaderMesh.m_vertexType == bqMDLChunkHeaderMesh::VertexType_Triangle)
						m.m_mtl.m_material.m_shaderType = bqShaderType::Standart;
					else
						m.m_mtl.m_material.m_shaderType = bqShaderType::StandartSkinned;
					m.m_mesh = newM;
					m.m_GPUmesh = gs->CreateMesh(newM);
					m.m_chunkHeaderMesh = chunkHeaderMesh;
					m_meshes.push_back(m);



					/*if (chunkHeaderMesh.m_vertexType == bqMDLChunkHeaderMesh::VertexType_Triangle)
					{
						printf("No weights\n");
					}
					else
					{
						bqVertexTriangleSkinned* V = (bqVertexTriangleSkinned*)newM->GetVBuffer();
						for (uint32_t o = 0; o < newM->GetInfo().m_vCount; ++o)
						{
							printf("%f %f %f %f %u %u %u %u\n", 
								V->Weights.x, 
								V->Weights.y, 
								V->Weights.z, 
								V->Weights.w,
								V->BoneInds.x,
								V->BoneInds.y,
								V->BoneInds.z,
								V->BoneInds.w);

							++V;
						}

					}*/

				}break;
				
				case bqMDLChunkHeader::ChunkType_CollisionMesh:
				{
					bqMDLChunkHeaderCollisionMesh chunkHeaderMesh;
					file.Read(&chunkHeaderMesh, sizeof(chunkHeaderMesh));

					m_aabb = chunkHeaderMesh.m_aabb.m_aabb;
					m_radius = chunkHeaderMesh.m_aabb.m_aabb.m_radius;

					uint32_t vSz = chunkHeaderMesh.m_vertNum * sizeof(bqVec3f);
					uint32_t iSz = chunkHeaderMesh.m_indNum * sizeof(uint32_t);

					if (chunkHeaderMesh.m_numOfBVHAabbs > 10000)
					{
						bqLog::PrintError("%s : too many aabbs for collision\n", BQ_FUNCTION);
						return false;
					}

					bqMDLCollision* newCollision = new bqMDLCollision;
					newCollision->m_aabb = m_aabb;
					newCollision->m_radius = m_radius;
					newCollision->m_vBuf = new bqVec3f[chunkHeaderMesh.m_vertNum];
					newCollision->m_iBuf = new uint32_t[chunkHeaderMesh.m_indNum];
					newCollision->m_bvhNodeNum = chunkHeaderMesh.m_numOfBVHAabbs;
					newCollision->m_bvh = new bqMDLBVHNode[newCollision->m_bvhNodeNum];
					newCollision->m_vertNum = chunkHeaderMesh.m_vertNum;
					newCollision->m_indNum = chunkHeaderMesh.m_indNum;

					file.Read(newCollision->m_vBuf, vSz);
					file.Read(newCollision->m_iBuf, iSz);

					for (size_t ai = 0; ai < chunkHeaderMesh.m_numOfBVHAabbs; ++ai)
					{
						file.Read(&newCollision->m_bvh[ai].m_mdl_aabb, sizeof(bqMDLBVHAABB));

						if (newCollision->m_bvh[ai].m_mdl_aabb.m_triNum)
						{
							file.Read(&newCollision->m_bvh[ai].m_triNum, sizeof(uint32_t));
							newCollision->m_bvh[ai].m_tris = new uint32_t[newCollision->m_bvh[ai].m_triNum];

							for (uint32_t ti = 0; ti < newCollision->m_bvh[ai].m_triNum; ++ti)
							{
								file.Read(&newCollision->m_bvh[ai].m_tris[ti], sizeof(uint32_t));
							}
						}
					}
					/*for (size_t li = 0; li < chunkHeaderMesh.m_numOfBVHLeaves; ++li)
					{
						
						file.Read(&newCollision->m_bvh[li].m_triNum, sizeof(uint32_t));

						newCollision->m_bvh[li].m_tris = new uint32_t[newCollision->m_bvh[li].m_triNum];

						for (int32_t ti = 0; ti < newCollision->m_bvh[li].m_triNum; ++ti)
						{
							file.Read(&newCollision->m_bvh[li].m_tris[ti], sizeof(uint32_t));
						}
					}*/

					m_collision = newCollision;
				}break;
				case bqMDLChunkHeader::ChunkType_HitboxMesh:
				{
					bqMDLChunkHeaderHitboxMesh chunkHeaderMesh;
					file.Read(&chunkHeaderMesh, sizeof(chunkHeaderMesh));

					if (chunkHeaderMesh.m_vNum)
					{
						uint32_t vSz = chunkHeaderMesh.m_vNum * sizeof(bqVec3f);
						uint32_t iSz = chunkHeaderMesh.m_vNum * sizeof(uint16_t);
						//chunkHeaderMesh.m_bone

						bqMDLHitbox* newHitbox = new bqMDLHitbox;
						newHitbox->m_aabb = chunkHeaderMesh.m_aabb;
						newHitbox->m_vBuf = new bqVec3f[chunkHeaderMesh.m_vNum];
						newHitbox->m_iBuf = new uint16_t[chunkHeaderMesh.m_vNum];
						newHitbox->m_vNum = chunkHeaderMesh.m_vNum;
						newHitbox->m_jointIndex = chunkHeaderMesh.m_bone;

						file.Read(newHitbox->m_vBuf, vSz);
						file.Read(newHitbox->m_iBuf, iSz);

						m_hitboxes.push_back(newHitbox);
					}
				}break;
				case bqMDLChunkHeader::ChunkType_String:
				{
					bqMDLChunkHeaderString chunkHeaderString;
					file.Read(&chunkHeaderString, sizeof(chunkHeaderString));
					if (!chunkHeaderString.m_strSz)
					{
						bqLog::PrintError("bqMDL: bad string size\n");
						return false;
					}
					
					straArray.clear();
					straArray.reserve(chunkHeaderString.m_strSz);
					straArray.m_size = chunkHeaderString.m_strSz;

					file.Read(straArray.m_data, straArray.m_size);
					
					stra.clear();
					stra.append(straArray.m_data, straArray.m_size);
					m_strings.push_back(stra);

				}break;

				case bqMDLChunkHeader::ChunkType_Skeleton:
				{
					bqMDLChunkHeaderSkeleton chunkHeaderSkeleton;
					file.Read(&chunkHeaderSkeleton, sizeof(chunkHeaderSkeleton));
					if (!chunkHeaderSkeleton.m_boneNum)
					{
						bqLog::PrintError("bqMDL: bad m_boneNum\n");
						return false;
					}

					m_skeleton = new bqSkeleton;

					for (uint32_t bi = 0; bi < chunkHeaderSkeleton.m_boneNum; ++bi)
					{
						bqMDLBoneData boneData;
						file.Read(&boneData, sizeof(boneData));

						m_skeleton->AddJoint(
							bqQuaternion(boneData.m_rotation[0],
								boneData.m_rotation[1],
								boneData.m_rotation[2],
								boneData.m_rotation[3]), 
							bqVec4(boneData.m_position[0],
								boneData.m_position[1],
								boneData.m_position[2],
								0.0), 
							bqVec4(boneData.m_scale[0],
								boneData.m_scale[1],
								boneData.m_scale[2],
								0.0),
							m_strings[boneData.m_nameIndex].c_str(),
							boneData.m_parent);


						/*printf("BONE[%u] POS:[%f %f %f]\n", bi,
							boneData.m_position[0],
							boneData.m_position[1],
							boneData.m_position[2]);*/
					}

					
				}break;

				case bqMDLChunkHeader::ChunkType_Material:
				{
					bqMDLChunkHeaderMaterial chunkHeaderMaterial;
					file.Read(&chunkHeaderMaterial, sizeof(chunkHeaderMaterial));
					materials.push_back(chunkHeaderMaterial);
				}break;

				case bqMDLChunkHeader::ChunkType_Animation:
				{
					bqMDLChunkHeaderAnimation chunkHeaderAnimation;
					file.Read(&chunkHeaderAnimation, sizeof(chunkHeaderAnimation));

					if (!m_skeleton)
					{
						bqLog::PrintError("bqMDL: skeleton not exist at this moment\n");
						return false;
					}
					else if(m_skeleton->GetJointNum() == 0)
					{
						bqLog::PrintError("bqMDL: skeleton has 0 bones\n");
						return false;
					}

					if (!chunkHeaderAnimation.m_framesNum)
					{
						bqLog::PrintError("bqMDL: chunkHeaderAnimation.m_framesNum\n");
						return false;
					}
					
					bqSkeletonAnimation* ani = 
						new bqSkeletonAnimation(
							m_skeleton->GetJointNum(),
							chunkHeaderAnimation.m_framesNum,
							m_strings.m_data[chunkHeaderAnimation.m_nameIndex].c_str());
					if (ani)
					{
						size_t boneNum = m_skeleton->GetJointNum();
						auto & joints = m_skeleton->GetJoints();

						// надо инициализировать bqJointBase
						// который внутри ani
						for (size_t bi = 0; bi < boneNum; ++bi)
						{
							bqJoint* joint = &joints.m_data[bi];
							bqJointBase JB = joint->m_base;
							ani->SetJoint(&JB, bi);
						}

						for (size_t fi = 0; fi < chunkHeaderAnimation.m_framesNum; ++fi)
						{
							for (size_t bi = 0; bi < boneNum; ++bi)
							{
								bqJoint* joint = &joints.m_data[bi];

								// чанк с анимациями такой
								/*
								* bqMDLChunkHeaderAnimation (напр. 3 фреймма и 2 кости)
								* bqMDLAnimationData - joint0 frame0
								* bqMDLAnimationData - joint1
								* bqMDLAnimationData - joint2
								* bqMDLAnimationData - joint0 frame1
								* bqMDLAnimationData - joint1
								* bqMDLAnimationData - joint2
								* bqMDLAnimationData - joint0 frame2
								* bqMDLAnimationData - joint1
								* bqMDLAnimationData - joint2
								*/
								bqMDLAnimationData ad;
								file.Read(&ad, sizeof(ad));

								bqJointTransformationBase JT;
								JT.m_position.x = ad.m_position[0];
								JT.m_position.y = ad.m_position[1];
								JT.m_position.z = ad.m_position[2];

								JT.m_scale.x = ad.m_scale[0];
								JT.m_scale.y = ad.m_scale[1];
								JT.m_scale.z = ad.m_scale[2];

								//bqQuaternion qX(ad.m_rotation[0], 0.f, 0.f);
								//bqQuaternion qY(0.f, ad.m_rotation[1], 0.f);
								//bqQuaternion qZ(0.f, 0.f, ad.m_rotation[2]);
								//bqQuaternion qR = qX * qY * qZ;

								JT.m_rotation.x = ad.m_rotation[0];
								JT.m_rotation.y = ad.m_rotation[1];
								JT.m_rotation.z = ad.m_rotation[2];
								JT.m_rotation.w = ad.m_rotation[3];

								ani->SetTransformation(bi, fi, JT);
							}
						}

						m_loadedAnimations.push_back(ani);
					}

				}break;

				default:
					bqLog::PrintError("bqMDL: bad chunk type [%u]\n", chunkHeader.m_chunkType);
					return false;
				}
			}
		}

		if (free_bqMesh)
			FreebqMesh();

		if (m_skeleton)
		{
			m_skeleton->m_preRotation.SetRotation(
				bqQuaternion(fileHeader.m_rotation[0], 
					fileHeader.m_rotation[1], 
					fileHeader.m_rotation[2]));

			bqMat4 mS = bqMath::ScaleMatrix(fileHeader.m_scale);

			m_skeleton->m_preRotation = m_skeleton->m_preRotation * mS;

			m_skeleton->CalculateBind();
			m_skeleton->Update();

			
			/*auto & joints = m_skeleton->GetJoints();
			for (size_t i = 0; i < joints.m_size; ++i)
			{
				auto& joint = joints.m_data[i];

				if (joint.m_base.m_parentIndex != -1)
					printf("Bone [%s] Parent [%s]\n", joint.m_base.m_name, joints.m_data[joint.m_base.m_parentIndex].m_base.m_name);
				else
					printf("Bone [%s] is root\n", joint.m_base.m_name);
			}*/

			//if(m_loadedAnimations.size())
				//m_skeletonForAnimation = m_skeleton->Duplicate();
		}

		if (materials.m_size)
		{
			for (size_t i = 0; i < materials.m_size; ++i)
			{
				bqMDLChunkHeaderMaterial& m = materials.m_data[i];
				printf("Material[%u][%s]\n", i, m_strings[m.m_nameIndex].c_str());

				if(m.m_difMap != -1)
					printf("\tdiffuse map: [%s]\n", m_strings[m.m_difMap].c_str());
			}

			for (size_t i = 0; i < m_meshes.m_size; ++i)
			{
				_mesh& m = m_meshes.m_data[i];


				m.m_mtl.m_header = *l_getMaterialHeader(m.m_chunkHeaderMesh.m_material);

				m.m_mtl.m_material.m_colorAmbient.SetR(m.m_mtl.m_header.m_ambient[0]);
				m.m_mtl.m_material.m_colorAmbient.SetG(m.m_mtl.m_header.m_ambient[1]);
				m.m_mtl.m_material.m_colorAmbient.SetB(m.m_mtl.m_header.m_ambient[2]);

				m.m_mtl.m_material.m_colorDiffuse.SetR(m.m_mtl.m_header.m_diffuse[0]);
				m.m_mtl.m_material.m_colorDiffuse.SetG(m.m_mtl.m_header.m_diffuse[1]);
				m.m_mtl.m_material.m_colorDiffuse.SetB(m.m_mtl.m_header.m_diffuse[2]);

				m.m_mtl.m_material.m_colorSpecular.SetR(m.m_mtl.m_header.m_specular[0]);
				m.m_mtl.m_material.m_colorSpecular.SetG(m.m_mtl.m_header.m_specular[1]);
				m.m_mtl.m_material.m_colorSpecular.SetB(m.m_mtl.m_header.m_specular[2]);
				
			//	printf("%f %f %f\n", m.m_mtl.m_header.m_ambient[0], m.m_mtl.m_header.m_ambient[1], m.m_mtl.m_header.m_ambient[2]);
			//	printf("%f %f %f\n", m.m_mtl.m_header.m_diffuse[0], m.m_mtl.m_header.m_diffuse[1], m.m_mtl.m_header.m_diffuse[2]);
			//	printf("%f %f %f\n", m.m_mtl.m_header.m_specular[0], m.m_mtl.m_header.m_specular[1], m.m_mtl.m_header.m_specular[2]);

				bqTextureInfo ti;
				//ti.m_filter = ;
				bqStringA imgPath;
				if (textureDir)
					imgPath.append(textureDir);

				if (imgPath.size())
				{
					char* str = imgPath.data();
					if ((str[imgPath.size() - 1] != '/')
						&& (str[imgPath.size() - 1] != '\\'))
						imgPath.push_back('/');
				}
				else
				{
					imgPath.push_back('/');
				}
				
				int32_t difMapIndex = m.m_mtl.m_header.m_difMap;
				if (difMapIndex != -1)
				{
					imgPath.append(m_strings[difMapIndex].c_str());
				}

				if (imgPath.size())
				{
					bqImage* img = bqFramework::CreateImage(imgPath.c_str());
					if (img)
					{
						m.m_mtl.m_diffuseMap = gs->CreateTexture(img, ti);
						if (m.m_mtl.m_diffuseMap)
						{
							m.m_mtl.m_material.m_maps[0].m_texture = m.m_mtl.m_diffuseMap;


							m.m_mtl.m_material.m_shaderType = bqShaderType::Standart;
							if(m.m_chunkHeaderMesh.m_vertexType == 
								bqMDLChunkHeaderMesh::VertexType_TriangleSkinned)
								m.m_mtl.m_material.m_shaderType = bqShaderType::StandartSkinned;
						}


						delete img;
					}
				}
			}
		}

		printf("Load end... %zu meshes %zu materials %zu animations\n", 
			m_meshes.m_size, 
			materials.m_size,
			m_loadedAnimations.m_size);


		return true;
	}
	return false;
}

void bqMDL::Unload()
{
	if (m_compressedData)
	{
		free(m_compressedData);
		m_compressedData = 0;
	}

	FreebqMesh();
	for (uint32_t i = 0; i < m_meshes.m_size; ++i)
	{
		BQ_SAFEDESTROY(m_meshes.m_data[i].m_GPUmesh);
		
		BQ_SAFEDESTROY(m_meshes.m_data[i].m_mtl.m_diffuseMap);
	}
	for (size_t i = 0; i < m_loadedAnimations.m_size; ++i)
	{
		BQ_SAFEDESTROY(m_loadedAnimations.m_data[i]);
	}
	m_loadedAnimations.clear();
	m_meshes.clear();
	m_strings.clear();

//	BQ_SAFEDESTROY(m_skeletonForAnimation);
	BQ_SAFEDESTROY(m_skeleton);
	BQ_SAFEDESTROY(m_collision);
	//BQ_SAFEDESTROY(m_skeletonAnimationObject);
}

void bqMDL::FreebqMesh()
{
	for (uint32_t i = 0; i < m_meshes.m_size; ++i)
	{
		BQ_SAFEDESTROY(m_meshes.m_data[i].m_mesh);
	}
}

bqMDLCollision::bqMDLCollision()
{
}

bqMDLCollision::~bqMDLCollision()
{
	BQ_SAFEDESTROY_A(m_vBuf);
	BQ_SAFEDESTROY_A(m_iBuf);
	BQ_SAFEDESTROY_A(m_bvh);
}

bool bqMDLCollision::CollisionSphereSphere(bqReal radius, const bqVec3& origin)
{
	bqReal d = bqMath::Distance(origin, bqZeroVector3);
	bqReal R = radius + m_radius;
	if (d < R)
	{
		bqReal len = R - d;
		m_outNormal = origin;
		m_outNormal.Normalize();
		m_outIntersection = origin - (m_outNormal * len);
		m_outLen = len;

		return true;
	}
	return false;
}
bool bqMDLCollision::CollisionSphereBox(bqReal radius, const bqVec3& origin)
{
	return m_aabb.SphereIntersect(origin, radius);
}
bool bqMDLCollision::CollisionSphereBVH(bqReal radius, const bqVec3& origin)
{
	BQ_ASSERT_ST(m_bvh);
	if (m_bvhNodeNum)
	{
		bqMDLBVHNode* currNode = m_bvh;
		for (uint32_t i = 0; i < m_bvhNodeNum; ++i)
		{
			if (currNode->m_mdl_aabb.m_aabb.SphereIntersect(origin, radius))
			{

			}
		}
	}
	return false;
}
bool bqMDLCollision::CollisionSphereTriangle(bqReal radius, const bqVec3& origin)
{
	bqTriangle tri;
	for (uint32_t i = 0; i < m_indNum; )
	{
		tri.v1 = m_vBuf[m_iBuf[i]];
		tri.v2 = m_vBuf[m_iBuf[i+1]];
		tri.v3 = m_vBuf[m_iBuf[i+2]];
		
		tri.Update();

		bqReal len = 0.0;
		bqVec3 ip;
		if (tri.SphereIntersect(radius, origin, len, ip))
		{
			m_outIntersection = ip;
			m_outNormal = tri.normal;
			m_outLen = len;
			return true;
		}
		
		i += 3;
	}
	return false;
}

bool bqMDLCollision::CollisionTriangleTriangle(bqTriangle* t, const bqVec3& origin)
{
	bqTriangle tri;
	bqTriangle tri2 = *t;
	tri2.v1 += origin;
	tri2.v2 += origin;
	tri2.v3 += origin;

	for (uint32_t i = 0; i < m_indNum; )
	{
		tri.v1 = m_vBuf[m_iBuf[i]];
		tri.v2 = m_vBuf[m_iBuf[i + 1]];
		tri.v3 = m_vBuf[m_iBuf[i + 2]];

		tri.Update();

		bqReal len = 0.0;
		bqVec3 ip;
		if (tri.TriangleIntersect(&tri2))
		{
			m_outIntersection = ip;
			m_outNormal = tri.normal;
			m_outLen = len;
			m_outTriangle = tri;
			return true;
		}

		i += 3;
	}
	return false;
}

bool bqMDLCollision::CollisionRayTriangle(const bqRay& ray)
{
	bqTriangle tri;

	m_outLen = FLT_MAX;

	bool result = false;

	for (uint32_t i = 0; i < m_indNum; )
	{
		tri.v1 = m_vBuf[m_iBuf[i]];
		tri.v2 = m_vBuf[m_iBuf[i + 1]];
		tri.v3 = m_vBuf[m_iBuf[i + 2]];

		tri.Update();

		bqReal T, U, V, W;
		T = U = V = W = 0;

		bqVec3 ip;
		if (tri.RayIntersect_MT(ray, false, T, U, V, W))
		{
			if (T < m_outLen)
			{
				m_outIntersection = ray.m_origin + (ray.m_direction * T);
				m_outNormal = tri.normal;
				m_outLen = T;
				m_outTriangle = tri;
				result = true;
			}
		}

		i += 3;
	}
	return result;
}

//bool bqMDLCollision::CollisionAabbTriangle(const bqAabb& a, const bqVec3& origin)
//{
//	bqTriangle tri;
//
//	m_outLen = FLT_MAX;
//
//	bool result = false;
//
//	for (uint32_t i = 0; i < m_indNum; )
//	{
//		tri.v1 = m_vBuf[m_iBuf[i]];
//		tri.v2 = m_vBuf[m_iBuf[i + 1]];
//		tri.v3 = m_vBuf[m_iBuf[i + 2]];
//
//		tri.Update();
//
//		bqReal T, U, V, W;
//		T = U = V = W = 0;
//
//		bqVec3 ip;
//		if (tri.RayIntersect_MT(ray, false, T, U, V, W))
//		{
//			if (T < m_outLen)
//			{
//				m_outIntersection = ray.m_origin + (ray.m_direction * T);
//				m_outNormal = tri.normal;
//				m_outLen = T;
//				m_outTriangle = tri;
//				result = true;
//			}
//		}
//
//		i += 3;
//	}
//	return result;
//}

bqMDLBVHNode::bqMDLBVHNode()
{
}
bqMDLBVHNode::~bqMDLBVHNode()
{
	BQ_SAFEDESTROY_A(m_tris);
}

bqMDLHitbox::bqMDLHitbox()
{
}
bqMDLHitbox::~bqMDLHitbox()
{
	BQ_SAFEDESTROY_A(m_vBuf);
	BQ_SAFEDESTROY_A(m_iBuf);
}

#endif
