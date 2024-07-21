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

				default:
					bqLog::PrintError("bqMDL: bad chunk type [%u]\n", chunkHeader.m_chunkType);
					return false;
				}
			}
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
	if (m_compressedData)
	{
		free(m_compressedData);
		m_compressedData = 0;
	}

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
