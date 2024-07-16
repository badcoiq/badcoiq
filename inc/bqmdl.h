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

#pragma once
#ifndef __BQ_BQMDL_H__
#define __BQ_BQMDL_H__

#include <stdint.h>

struct bqMDLMeshVertex
{
	float m_position[3];
	float m_uv[2];
	float m_normal[3];
	float m_binormal[3];
	float m_tangent[3];
	float m_color[4];
	uint8_t m_boneInds[4];
	float m_weights[4];
};

/*
Структура файла

FILE HEADER
CHUNK HEADER
CHUNK DATA
CHUNK HEADER
CHUNK DATA
...
CHUNK HEADER
CHUNK DATA
TEXT TABLE HEADER
TEXT TABLE

*/

struct bqMDLFileHeader
{
	uint32_t m_bmld = 1818520930; // "bmdl"
	uint16_t m_version = 1;
	uint32_t m_crc = 0;
	uint16_t m_chunkNum = 0;
	uint32_t m_textTableOffset = 0;
	uint32_t m_reserved1 = 0;
	uint32_t m_reserved2 = 0;
	uint32_t m_reserved3 = 0;
	uint32_t m_reserved4 = 0;
};

struct bqMDLChunkHeader
{
	// 1 - mesh chunk
	uint16_t m_chunkType = 0;
	uint32_t m_reserved1 = 0;
	uint32_t m_reserved2 = 0;
};

// дальше идёт зависимое от типа чанка (m_chunkType)

struct bqMDLChunkHeaderMesh
{
	uint16_t m_name = 0;
	
	// 0 - 16bit
	// 1 - 32bit
	uint8_t m_indexType = 0;
	
	uint8_t m_stride = sizeof(bqMDLMeshVertex);
	
	uint16_t m_material = 0;
	uint32_t m_vertNum = 0;
	uint32_t m_indNum = 0;
	uint32_t m_vertBufSz = 0;
	uint32_t m_indBufSz = 0;
	uint32_t m_reserved1 = 0;
	uint32_t m_reserved2 = 0;
};

struct bqMDLTextTableHeader
{
	// количество строк идущих после bqMDLTextTableHeader
	uint16_t m_strNum = 0;
	uint32_t m_reserved1 = 0;
};


// далее идут строки в соответствии с количеством строк
// из bqMDLTextTableHeader. Строки заканчиваются нулём.

#endif

