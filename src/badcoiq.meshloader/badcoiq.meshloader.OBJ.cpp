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
#include <vector>

// Для упрощения понимания.
// фейс (f) может содержать
enum class OBJFaceType
{
	p,   // только позицию
	pu,  // позицию и текстурную координату
	pun, // как pu + нормаль
	pn   // позиция и нормаль
};

// для ускорения чтения.
// если на фейс будет более 100 вершин то ошибка.
// к чёрту такие модели, или увеличивай data.
struct OBJSimpleArr
{
	OBJSimpleArr() {
		sz = 0;
	}

	int data[0x100];
	unsigned int sz;

	void push_back(int v) { data[sz++] = v; }
	unsigned int size() { return sz; }
	void reset() { sz = 0; }
};

// чтение фейса сюда
struct OBJFace
{
	OBJFace() {
		ft = OBJFaceType::pun;
	}

	OBJSimpleArr p, u, n;
	OBJFaceType ft;

	void reset()
	{
		ft = OBJFaceType::pun;
		p.reset();
		u.reset();
		n.reset();
	}
};

// читаю MTL сюда
struct OBJMaterial
{
	OBJMaterial() {
		m_specularExponent = 10.f;
		m_opacity = 1.f;
		m_transparency = 0.f;
		m_refraction = 1.f;
	}

	bqStringA m_name; // newmtl
	bqVec3f m_ambient;   // Ka
	bqVec3f m_diffuse;   // Kd
	bqVec3f m_specular;  // Ks
	float m_specularExponent; // Ns
	float m_opacity;    // d
	float m_transparency; // Tr
	float m_refraction;  // Ni

	bqStringA m_map_diffuse; // map_Kd  
	bqStringA m_map_ambient; // map_Ka 
	bqStringA m_map_specular; // map_Ks   
	bqStringA m_map_specularHighlight; // map_Ns    
	bqStringA m_map_alpha; // map_d     
	bqStringA m_map_bump; // map_bump bump 
	bqStringA m_map_displacement; // disp 
	bqStringA m_map_reflection; // refl  
};

// Чтение MTL
void bqMeshLoaderImpl::ImportOBJ_MTL(
	bqArray<OBJMaterial*>& materials,
	const char* obj_fileName,
	const char* mtl_fileName,
	bqMeshLoaderCallback* cb
)
{
	// .mtl находится рядом с .obj
	// надо получить путь
	// было
	// ../../data/3dmodels/box.obj
	// убираем до
	// ../../data/3dmodels/
	// вставляем mtl_fileName
	// ../../data/3dmodels/box.mtl

	bqString relPath = obj_fileName;

	for (size_t i = 0, sz = relPath.size(); i < sz; ++i)
	{
		auto c = relPath.pop_back_return();
		if (c == U'/' || c == U'\\')
		{
			relPath.push_back(c);
			break;
		}
	}

	bqString mtlPath = relPath;
	mtlPath += mtl_fileName;

	bqStringA utf8;
	mtlPath.to_utf8(utf8);
	
	// читаем файл или распаковываем
	uint32_t file_size = 0;
	uint8_t* ptr = bqFramework::SummonFileBuffer(utf8.c_str(), &file_size, true);
	if (ptr)
	{
		bqTextBufferReader tbr(ptr, file_size); // text buffer reader
		bqTextBufferReader lbr; // line buffer reader
		bqStringA line;
		bqStringA word;

		OBJMaterial* curMaterial = 0;
		
		while (!tbr.IsEnd())
		{
			tbr.SkipSpaces();

			char ch = tbr.PickChar();
			switch (ch)
			{
			case '#':
			case 'd':
			case 'T':
			case 'i':
			case 'K':
				tbr.SkipLine();
				break;
			case 'n':
			{
				tbr.GetWord(word, 0); // `newmtl`

				if (strcmp(word.c_str(), "newmtl") == 0)
				{
					curMaterial = new OBJMaterial;
					materials.push_back(curMaterial);

					tbr.GetWord(curMaterial->m_name, 0);
				}
			}break;
			case 'N':
			{
				tbr.GetWord(word, 0);

				if (strcmp(word.c_str(), "Ns") == 0)
				{
					tbr.SkipSpaces();
					if(curMaterial)
						curMaterial->m_specularExponent = tbr.GetFloat();
				}
				else if (strcmp(word.c_str(), "Ni") == 0)
				{
					tbr.SkipSpaces();
					if (curMaterial)
						curMaterial->m_refraction = tbr.GetFloat();
				}
				else
					tbr.SkipLine();

			}break;
			case 'm':
			{
				tbr.GetWord(word, 0);

				if (strcmp(word.c_str(), "map_Kd") == 0)
				{
					tbr.GetLine(line);

					bqString mapPath;
					mapPath.append(line.data());

					if (!bqFramework::FileExist(mapPath))
					{
						mapPath = relPath;
						mapPath.append(line.data());
					}

					if (curMaterial)
						mapPath.to_utf8(curMaterial->m_map_diffuse);
				}
				else
					tbr.SkipLine();
			}break;
			default:
				tbr.SkipLine();
				break;
			}
		}

		bqMemory::free(ptr);
	}
}

OBJMaterial* OBJGetMaterial(
	bqArray<OBJMaterial*>& materials,
	const bqStringA& name
)
{
	for (uint32_t i = 0; i < materials.m_size; ++i)
	{
		if (materials.m_data[i]->m_name == name)
			return materials.m_data[i];
	}
	return 0;
}

void bqMeshLoaderImpl::LoadOBJ(const char* path, bqMeshLoaderCallback* cb, uint8_t* buffer, uint32_t bufferSz)
{
	BQ_ASSERT_ST(cb);
	BQ_ASSERT_ST(buffer);
	BQ_ASSERT_ST(bufferSz);

	bqArray<OBJMaterial*> obj_materials;

	bool groupBegin = false;
	bool isModel = false;
	bool grpFound = false;

	bqVec2f tcoords;
	bqVec3f pos;
	bqVec3f norm;

	std::vector<bqVec3f> position;
	std::vector<bqVec2f> uv;
	std::vector<bqVec3f> normal;

	position.reserve(0xffff);
	uv.reserve(0xffff);
	normal.reserve(0xffff);

	position.reserve(0xffff);
	uv.reserve(0xffff);
	normal.reserve(0xffff);

	bqStringA tmp_word;
	bqString name;

	OBJFace f;

	int last_counter[3] = { 0,0,0 };

	bqMeshPolygonCreator polygonCreator;
	bqPolygonMesh* polygonMesh = 0;

	OBJMaterial* currMaterial = 0;

	bqTextBufferReader tbr(buffer, bufferSz); // text buffer reader
	bqTextBufferReader lbr; // line buffer reader
	bqStringA line;
	bqStringA word;

	while (!tbr.IsEnd())
	{
		tbr.SkipSpaces();
		char ch = tbr.PickChar();

		switch (ch)
		{
		case 'm'://mtllib
		{
			tbr.GetWord(word, 0); // `mtllib`

			if (strcmp(word.c_str(), "mtllib") == 0)
			{
				tbr.GetWord(word, 0);
				ImportOBJ_MTL(obj_materials, path, word.data(), cb);
			}
		}break;
		case 'u'://usemtl
		{
			tbr.GetWord(word, 0);

			if (strcmp(word.c_str(), "usemtl") == 0)
			{
				tbr.GetWord(word, 0);
				currMaterial = OBJGetMaterial(obj_materials, word);
				/*if (currMaterial)
				{
					bqMaterial m;
					m.m_name = currMaterial->m_name.c_str();

					cb->OnMaterial(&m);
				}*/
			}
		}break;
		case 's':
		case 'l':
		case 'c'://curv
		case 'p'://parm
		case 'd'://deg
		case '#':
		case 'e'://end
			tbr.SkipLine();
			break;
		case 'v':
		{
			tbr.GetWord(word, 0);

			auto ptr = word.c_str();
			++ptr;

			if (groupBegin)
				groupBegin = false;
			switch (*ptr)
			{
			case 't':
				tbr.GetLine(line); // надо получить то что справа от vt
				                   // там может быть 2 или 3 значения
									// напр vt 0.4402683 0.0862148 0.0000000
				// используя lbr можно без напряга получить числа.
				// если что-то не то то вернётся 0.0

				if (line.data())
				{
					lbr.Set(line.data(), line.size());
					
					tcoords.x = lbr.GetFloat();
					tcoords.y = lbr.GetFloat();
				}


				uv.push_back(tcoords);
				++last_counter[1];
				break;
			case 'n':
				tbr.GetLine(line);
				if (line.data())
				{
					lbr.Set(line.data(), line.size());

					norm.x = lbr.GetFloat();
					norm.y = lbr.GetFloat();
					norm.z = lbr.GetFloat();
				}

				normal.push_back(norm);
				++last_counter[2];
				break;
			default:
				//ptr = OBJReadVec3(ptr, pos);

				tbr.GetLine(line);
				if (line.data())
				{
					lbr.Set(line.data(), line.size());

					pos.x = lbr.GetFloat();
					pos.y = lbr.GetFloat();
					pos.z = lbr.GetFloat();
				}

				position.push_back(pos);
				++last_counter[0];
				break;
			}
		}break;
		case 'f':
		{
			isModel = true;
			f.reset();
			//ptr = OBJReadFace(++ptr, f, s);

			tbr.GetWord(word, 0); // `f`

			tbr.GetLine(line);
			if (line.data())
			{
				lbr.Set(line.data(), line.size());

				while (!lbr.IsEnd())
				{
					int p = 1;
					int u = 1;
					int n = 1;

					p = lbr.GetInt();
					if (lbr.PickChar() == '/')
					{
						lbr.GetChar(); // skip `/`

						if (lbr.PickChar() == '/') // position and normal like this 'f 1//1'
						{
							lbr.GetChar(); // skip `/`

							n = lbr.GetInt();
							f.ft = OBJFaceType::pn;
						}
						else
						{
							u = lbr.GetInt();

							if (lbr.PickChar() == '/')
							{
								lbr.GetChar(); // skip `/`
								n = lbr.GetInt();
								f.ft = OBJFaceType::pun;
							}
							else
							{
								f.ft = OBJFaceType::pu;
							}
						}
					}
					else
					{
						f.ft = OBJFaceType::p;
					}

					f.n.push_back(n - 1);
					f.u.push_back(u - 1);
					f.p.push_back(p - 1);

					lbr.SkipSpaces();
				}
			}

			polygonCreator.Clear();

			bool genNormals = true;

			for (size_t sz2 = f.p.size(), i2 = 0; i2 < sz2; ++i2)
			{
				auto index = i2;
				auto pos_index = f.p.data[index];


				if (pos_index < 0)
				{
					// если индекс отрицательный то он указывает на позицию относительно последнего элемента
					// -1 = последний элемент
					// if index is negative then he points on position relative last element
					// -1 = last element
					pos_index = last_counter[0] + pos_index + 1;
				}

				auto v = position[pos_index];

				bqVec3f pcPos, pcNorm;
				bqVec2f pcUV;

				pcPos = v;

				if (f.ft == OBJFaceType::pu || f.ft == OBJFaceType::pun)
				{
					auto uv_index = f.u.data[index];

					if (uv_index < 0)
					{
						uv_index = last_counter[1] + uv_index + 1;
					}

					auto u = uv[uv_index];
					pcUV.x = u.x;
					pcUV.y = 1.f - u.y;
				}

				if (f.ft == OBJFaceType::pn || f.ft == OBJFaceType::pun)
				{
					auto nor_index = f.n.data[index];

					if (nor_index < 0)
					{
						nor_index = last_counter[2] + nor_index + 1;
					}

					auto n = normal[nor_index];
					pcNorm = -n;
					genNormals = false;
				}
				polygonCreator.SetPosition(pcPos);
				polygonCreator.SetNormal(pcNorm);
				polygonCreator.SetUV(pcUV);
				polygonCreator.AddVertex();
			}

			if (!polygonMesh)
				polygonMesh = bqFramework::SummonPolygonMesh();
			polygonMesh->AddPolygon(&polygonCreator, genNormals);

		}break;
		case 'o':
		case 'g':
		{
			if (!groupBegin)
				groupBegin = true;
			else
			{
				tbr.SkipLine();
				break;
			}

			tbr.GetWord(tmp_word, 0); //skip o or g
			tbr.GetWord(tmp_word, 0); // now get name

			if (tmp_word.size())
			{
				name.assign(tmp_word.data());
			}

			if (grpFound)
			{
				polygonMesh = _obj_createModel(cb, &name, polygonMesh, currMaterial);
			}
			grpFound = true;
		}break;
		default:
			tbr.SkipLine();
			break;
		}
	}

	if (polygonMesh)
	{
		polygonMesh = _obj_createModel(cb, &name, polygonMesh, currMaterial);
	}

	cb->Finale();

	for (uint32_t i = 0; i < obj_materials.m_size; ++i)
	{
		delete obj_materials.m_data[i];
	}
}

void bqMeshLoaderImpl::LoadOBJ(const char* path, bqMeshLoaderCallback* cb)
{
	BQ_ASSERT_ST(path);
	BQ_ASSERT_ST(cb);

	uint32_t file_size = 0;
	uint8_t* ptr = bqFramework::SummonFileBuffer(path, &file_size, true);
	if (ptr)
	{
		LoadOBJ(path, cb, ptr, (uint32_t)file_size);
		bqDestroy(ptr);
	}
}

bqPolygonMesh* bqMeshLoaderImpl::_obj_createModel(
	bqMeshLoaderCallback* cb,
	bqString* name,
	bqPolygonMesh* polygonMesh, 
	OBJMaterial* currMaterial)
{
	if (polygonMesh->m_polygons.m_head)
	{
		bqMaterial* m = 0;
		bqString matName;
		if (currMaterial)
		{
			// тут надо будет заполнить и передать bqMaterial
			bqMaterial m;
			m.m_name = currMaterial->m_name.c_str();
			sprintf_s((char*)m.m_maps[0].m_filePath, sizeof(m.m_maps[0].m_filePath), "%s", (const char*)currMaterial->m_map_diffuse.c_str());
			matName = m.m_name;

			cb->OnMaterial(&m);
		}

		cb->OnMesh(polygonMesh->SummonMesh(), name, &matName);

		bqDestroy(polygonMesh);
		polygonMesh = 0;
	}

	return polygonMesh;
}

