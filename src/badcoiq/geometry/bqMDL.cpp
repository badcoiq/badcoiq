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


bqMDL::bqMDL()
{
}

bqMDL::~bqMDL()
{
	Unload();
}

bool bqMDL::Load(const char* fn, bqGS* gs, bool free_bqMesh)
{
	Unload();

	BQ_ASSERT_ST(fn);
	BQ_ASSERT_ST(gs);
	if (fn && gs)
	{
		uint32_t fileSz = 0;
		uint8_t* ptr = bqFramework::SummonFileBuffer(fn, &fileSz, false);
		if (ptr)
		{
			// для автоматического уничтожения ptr
			BQ_PTR_D(uint8_t, ptrr, ptr);

			bqFileBuffer file(ptr, fileSz);
			
			bqMDLFileHeader fileHeader;
			bqMDLChunkHeader chunkHeader;
			bqMDLChunkHeaderMesh chunkHeaderMesh;
			bqMDLTextTableHeader textTableHeader;

			file.Read(&fileHeader.m_bmld, sizeof(fileHeader.m_bmld));
			file.Read(&fileHeader.m_version, sizeof(fileHeader.m_version));
			file.Read(&fileHeader.m_chunkNum, sizeof(fileHeader.m_chunkNum));
			file.Read(&fileHeader.m_reserved1, sizeof(fileHeader.m_reserved1));
			file.Read(&fileHeader.m_reserved2, sizeof(fileHeader.m_reserved2));
			file.Read(&fileHeader.m_reserved3, sizeof(fileHeader.m_reserved3));
			file.Read(&fileHeader.m_reserved4, sizeof(fileHeader.m_reserved4));

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

			for (uint32_t i = 0; i < fileHeader.m_chunkNum; ++i)
			{
				file.Read(&chunkHeader.m_chunkType, sizeof(chunkHeader.m_chunkType));
				file.Read(&chunkHeader.m_reserved1, sizeof(chunkHeader.m_reserved1));
				file.Read(&chunkHeader.m_reserved2, sizeof(chunkHeader.m_reserved2));

				switch (chunkHeader.m_chunkType)
				{
				case 1: //bqMDLChunkHeaderMesh
				{
					file.Read(&chunkHeaderMesh.m_name, sizeof(chunkHeaderMesh.m_name));
					file.Read(&chunkHeaderMesh.m_indexType, sizeof(chunkHeaderMesh.m_indexType));
					file.Read(&chunkHeaderMesh.m_vertexType, sizeof(chunkHeaderMesh.m_vertexType));
					file.Read(&chunkHeaderMesh.m_material, sizeof(chunkHeaderMesh.m_material));
					file.Read(&chunkHeaderMesh.m_vertNum, sizeof(chunkHeaderMesh.m_vertNum));
					file.Read(&chunkHeaderMesh.m_indNum, sizeof(chunkHeaderMesh.m_indNum));
					file.Read(&chunkHeaderMesh.m_vertBufSz, sizeof(chunkHeaderMesh.m_vertBufSz));
					file.Read(&chunkHeaderMesh.m_indBufSz, sizeof(chunkHeaderMesh.m_indBufSz));
					file.Read(&chunkHeaderMesh.m_reserved1, sizeof(chunkHeaderMesh.m_reserved1));
					file.Read(&chunkHeaderMesh.m_reserved2, sizeof(chunkHeaderMesh.m_reserved2));

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

					//uint8_t* m_vertices = nullptr;
					//uint8_t* m_indices = nullptr;
					bqMesh* newM = new bqMesh;
					newM->Allocate(chunkHeaderMesh.m_vertNum,
						chunkHeaderMesh.m_indNum,
						(chunkHeaderMesh.m_vertexType == 0) ? false : true);

					file.Read(newM->GetVBuffer(), chunkHeaderMesh.m_vertBufSz);
					file.Read(newM->GetIBuffer(), chunkHeaderMesh.m_indBufSz);

					//newM->GenerateNormals(true);

					_mesh m;
					m.m_mesh = newM;
					m.m_GPUmesh = gs->SummonMesh(newM);
					m.m_chunkHeaderMesh = chunkHeaderMesh;
					m_meshes.push_back(m);
				}break;

				default:
					bqLog::PrintError("bqMDL: bad chunk type [%u]\n", chunkHeader.m_chunkType);
					return false;
				}
			}

			// в конце идут строки
			file.Read(&textTableHeader.m_strNum, sizeof(textTableHeader.m_strNum));
			file.Read(&textTableHeader.m_reserved1, sizeof(textTableHeader.m_reserved1));
			
			bqStringA stra;
			/*
			for (uint32_t i = 0; i < textTableHeader.m_strNum; ++i)
			{
			}*/
			while (!file.Eof())
			{
				uint8_t byte = 0;
				file.Read(&byte, 1);

				if (!byte)
				{
					if (stra.size())
					{
						m_strings.push_back(stra);
						stra.clear();
					}
				}
				else
				{
					stra.push_back(byte);
				}
			}
			//bqDestroy(ptr);
			if (stra.size())
				m_strings.push_back(stra);
		}

		if (free_bqMesh)
			FreebqMesh();

		printf("Load end... %u meshes\n", m_meshes.m_size);
		return true;
	}
	return false;
}

void bqMDL::Unload()
{
	FreebqMesh();
	for (uint32_t i = 0; i < m_meshes.m_size; ++i)
	{
		BQ_SAFEDESTROY(m_meshes.m_data[i].m_GPUmesh);
	}
	m_meshes.clear();
	m_strings.clear();
}

void bqMDL::FreebqMesh()
{
	for (uint32_t i = 0; i < m_meshes.m_size; ++i)
	{
		BQ_SAFEDESTROY(m_meshes.m_data[i].m_mesh);
	}
}


#endif
