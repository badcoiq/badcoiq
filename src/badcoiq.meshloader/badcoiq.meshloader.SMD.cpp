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

#include "badcoiq.meshloader.h"

#include "badcoiq/common/bqTextBufferReader.h"
#include "badcoiq/geometry/bqMeshCreator.h"
#include "badcoiq/geometry/bqSkeleton.h"

#include <filesystem>

void bqMeshLoaderImpl::LoadSMD(const char* path, bqMeshLoaderCallback* cb)
{
	BQ_ASSERT_ST(path);
	BQ_ASSERT_ST(cb);

	uint32_t file_size = 0;
	
	//uint8_t* ptr = bqFramework::CreateFileBuffer(path, &file_size, true);
	//if (ptr)
	//bqPtr<uint8_t> ptr(bqFramework::CreateFileBuffer(path, &file_size, true), bqPtr<uint8_t>::Destroy());
	BQ_PTR_D(uint8_t, ptr, bqFramework::CreateFileBuffer(path, &file_size, true));
	if (ptr.Ptr())
	{
		//LoadSMD(path, cb, ptr, (uint32_t)file_size);
		LoadSMD(path, cb, ptr.Ptr(), (uint32_t)file_size);
		//bqDestroy(ptr);
	}
}

void bqMeshLoaderImpl::LoadSMD(const char* path, bqMeshLoaderCallback* cb, uint8_t* buffer, uint32_t bufferSz)
{
	BQ_ASSERT_ST(cb);
	BQ_ASSERT_ST(buffer);
	BQ_ASSERT_ST(bufferSz);

	int errcode = 0;

	struct Nodes
	{
		int parentIndex = -1;
		bqStringA name;

		struct Frame
		{
			int index = 0;
			float position[3];
			float rotation[3];
		};
		bqArray<Frame> frames;
	};
	bqArray<Nodes> nodes;
	nodes.reserve(0x800);

	int frameCount = 0;

	struct Triangle
	{
		int nodeID[3];
		bqVec3f position[3];
		bqVec3f normal[3];
		bqVec2f UV[3];
	};
	struct Model
	{
		bqStringA textureName;
		bqArray<Triangle> triangles;
	};
	bqArray<Model> models;

	bqTextBufferReader tbr(buffer, bufferSz); // text buffer reader
	bqTextBufferReader lbr; // line buffer reader
	bqStringA line;
	bqStringA word;

	tbr.GetLine(line);
	lbr.Set(line.data(), line.size());

	lbr.GetWord(word, "");
	if (strcmp(word.c_str(), "version") == 0)
	{
		if (lbr.GetInt() == 1)
		{
			while (!tbr.IsEnd())
			{
				tbr.GetLine(line);

				if (strcmp(line.c_str(), "nodes") == 0)
				{
					while (!tbr.IsEnd())
					{
						tbr.GetLine(line);
						if (strcmp(line.c_str(), "end") == 0)
							break;

						lbr.Set(line.data(), line.size());

						int index = lbr.GetInt();
						lbr.GetString(word);
						int parentIndex = lbr.GetInt();

						Nodes newNode;
						newNode.parentIndex = parentIndex;
						newNode.name.assign(word.c_str());

						nodes.push_back(newNode);
					}
				}
				else if (strcmp(line.c_str(), "skeleton") == 0)
				{
					tbr.GetLine(line); // expect 'time' or 'end'
					lbr.Set(line.data(), line.size());
					lbr.PickWord(word);
					if (strcmp(word.c_str(), "time") == 0)
					{
					time:;
						++frameCount;

						for (size_t i = 0; i < nodes.m_size; ++i)
						{
							tbr.GetLine(line);
							if (strcmp(line.c_str(), "end") == 0)
								break;

							lbr.Set(line.data(), line.size());

							lbr.PickWord(word);
							if (strcmp(word.c_str(), "time") == 0)
								goto time; // next 'time'

							Nodes::Frame newFrame;
							newFrame.index = lbr.GetInt();
							newFrame.position[0] = lbr.GetFloat();
							newFrame.position[1] = lbr.GetFloat();
							newFrame.position[2] = lbr.GetFloat();
							newFrame.rotation[0] = lbr.GetFloat();
							newFrame.rotation[1] = lbr.GetFloat();
							newFrame.rotation[2] = lbr.GetFloat();

							nodes.m_data[i].frames.push_back(newFrame);

							tbr.PickWord(word);
							if (strcmp(word.c_str(), "time") == 0)
							{
								tbr.GetLine(line);
								goto time; // next 'time'
							}
						}
					}
				}
				else if (strcmp(line.c_str(), "triangles") == 0)
				{
					while (!tbr.IsEnd())
					{
						tbr.GetLine(line);
						if (strcmp(line.c_str(), "end") == 0)
							break;

						// здесь 'line' должна иметь имя текстуры

						// ищу model по имени текстуры
						// если ранее не было создано
						// то создаю
						Model* model = 0;
						for (size_t i = 0; i < models.m_size; ++i)
						{
							if (models.m_data[i].textureName == line)
							{
								model = &models.m_data[i];
								break;
							}
						}
						if (!model)
						{
							Model newModel;
							newModel.textureName = line;
							models.push_back(newModel);

							model = &models.m_data[models.m_size - 1];
						}

						Triangle newTriangle;
						for (int j = 0; j < 3; ++j)
						{
							tbr.GetLine(line);
							lbr.Set(line.data(), line.size());

							newTriangle.nodeID[j] = lbr.GetInt();
							newTriangle.position[j].x = lbr.GetFloat();
							newTriangle.position[j].y = lbr.GetFloat();
							newTriangle.position[j].z = lbr.GetFloat();
							newTriangle.normal[j].x = lbr.GetFloat();
							newTriangle.normal[j].y = lbr.GetFloat();
							newTriangle.normal[j].z = lbr.GetFloat();
							newTriangle.UV[j].x = lbr.GetFloat();
							newTriangle.UV[j].y = 1.f - lbr.GetFloat();
						}
						model->triangles.push_back(newTriangle);
					}

					/*printf("%i models\n", models.m_size);
					for (size_t i = 0; i < models.m_size; ++i)
					{
						printf("%s\n", models.m_data[i].textureName);
					}*/
				}
			}

			// if it `reference` then I need to create slSkeleton
			// `reference` is when I have model
			if (models.m_size)
			{
				bqMeshPolygonCreator polygonCreator;

				for (size_t i = 0; i < models.m_size; ++i)
				{
					// create mesh here
					Model* model = &models.m_data[i];

					BQ_PTR_D(bqPolygonMesh, polygonMesh, bqFramework::CreatePolygonMesh());
					
					for(size_t o = 0; o < model->triangles.m_size; ++o)
					{
						auto & tri = model->triangles.m_data[o];

						polygonCreator.Clear();

						polygonCreator.SetPosition(tri.position[0]);
						polygonCreator.SetNormal(tri.normal[0]);
						polygonCreator.SetUV(tri.UV[0]);
						polygonCreator.SetBoneInds(bqVec4_t<uint8_t>(tri.nodeID[0], 0, 0, 0));
						polygonCreator.SetBoneWeights(bqVec4f(1.f, 0.f, 0.f, 0.f));
						polygonCreator.AddVertex();

						polygonCreator.SetPosition(tri.position[1]);
						polygonCreator.SetNormal(tri.normal[1]);
						polygonCreator.SetUV(tri.UV[1]);
						polygonCreator.SetBoneInds(bqVec4_t<uint8_t>(tri.nodeID[1], 0, 0, 0));
						polygonCreator.SetBoneWeights(bqVec4f(1.f, 0.f, 0.f, 0.f));
						polygonCreator.AddVertex();

						polygonCreator.SetPosition(tri.position[2]);
						polygonCreator.SetNormal(tri.normal[2]);
						polygonCreator.SetUV(tri.UV[2]);
						polygonCreator.SetBoneInds(bqVec4_t<uint8_t>(tri.nodeID[2], 0, 0, 0));
						polygonCreator.SetBoneWeights(bqVec4f(1.f, 0.f, 0.f, 0.f));
						polygonCreator.AddVertex();

						polygonMesh.Ptr()->AddPolygon(&polygonCreator, false);
					}

					if (polygonMesh.Ptr()->m_polygons.m_head)
					{
						bqString name;
						name.assign(model->textureName.c_str());

						bqMaterial mat;
						memcpy_s(mat.m_maps[0].m_filePath, 0x1000, model->textureName.c_str(), model->textureName.size());
						mat.m_name = name;
						cb->OnMaterial(&mat);
						cb->OnMesh(polygonMesh.Ptr()->CreateMesh(true), &name, &name);
					}
				}

				bqVec4 P;
				bqSkeleton* skeleton = bqCreate<bqSkeleton>();
				for (size_t i = 0; i < nodes.m_size; ++i)
				{
					Nodes* node = &nodes.m_data[i];

					if (node->frames.size())
					{
						bqQuaternion qX(-node->frames[0].rotation[0], 0.f, 0.f);
						bqQuaternion qY(0.f, -node->frames[0].rotation[1], 0.f);
						bqQuaternion qZ(0.f, 0.f, -node->frames[0].rotation[2]);

						bqQuaternion qR = qX * qY * qZ;

						P.Set(node->frames[0].position[0],
							node->frames[0].position[1],
							node->frames[0].position[2], 1.f);

						skeleton->AddJoint(qR, P, bqVec4(1.0), node->name.c_str(), node->parentIndex);
					}
				}
				skeleton->m_preRotation.SetRotation(bqQuaternion(PI * 0.5f, 0.f, 0.f));
				cb->OnSkeleton(skeleton);
			}
			else if (frameCount && nodes.m_size)
			{
				// it probably animation

				std::filesystem::path p = path;
				std::filesystem::path fn = p.filename();
				if (fn.has_extension())
					fn.replace_extension();

				bqSkeletonAnimation* ani = bqCreate<bqSkeletonAnimation>(
					nodes.size(),
					frameCount,
					fn.generic_string().c_str());

				//printf("%i nodes, %i frames\n", nodes.size(), frameCount);

				// Set joints. Need to know name and parent index.
				for (size_t i = 0; i < nodes.m_size; ++i)
				{
					Nodes* node = &nodes.m_data[i];

					bqJointBase JB;
					strcpy_s(JB.m_name, sizeof(JB.m_name), node->name.c_str());
					JB.m_parentIndex = node->parentIndex;
					ani->SetJoint(&JB, i);
				}

				// Set frames.
				for (size_t i = 0; i < nodes.m_size; ++i)
				{
					Nodes* node = &nodes.m_data[i];
					for (size_t o = 0; o < (size_t)frameCount; ++o)
					{
						bqJointTransformationBase JT;
						JT.m_position.x = node->frames[o].position[0];
						JT.m_position.y = node->frames[o].position[1];
						JT.m_position.z = node->frames[o].position[2];

						bqQuaternion qX(-node->frames[o].rotation[0], 0.f, 0.f);
						bqQuaternion qY(0.f, -node->frames[o].rotation[1], 0.f);
						bqQuaternion qZ(0.f, 0.f, -node->frames[o].rotation[2]);
						bqQuaternion qR = qX * qY * qZ;

						JT.m_rotation = qR;

						ani->SetTransformation(i, o, JT);
					}
				}

				cb->OnAnimation(ani);
			}
		}
		else
		{
			errcode = 2;
			goto error;
		}
	}
	else
	{
		errcode = 1;
		goto error;
	}

	// НЕ надо
	// скорее всего нужно вызывать вручную
	//cb->Finale();

	return;

error:;
	bqLog::PrintWarning(L"SMD: error %i\n", errcode);
}
