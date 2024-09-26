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
#ifndef __BQ_BQMDLINFO_H__
#define __BQ_BQMDLINFO_H__

#include <stdint.h>

#include "badcoiq/geometry/bqAABB.h"
#include "badcoiq/math/bqVector.h"

// файл чтобы можно было подключить к проекту с плагином для 3Ds Max

// Вершина содержит такие данные. Аналогично
// с вершиной из bqMesh.h
struct bqMDLMeshVertex
{
	// позиция
	float m_position[3];

	// текстурные координаты
	float m_uv[2];

	// нормаль
	float m_normal[3];

	// для пиксельного освещения. Желательно
	// реализовывать генерацию в загрузчиках моделей
	// и в плагинах для экспорта .mdl.
	float m_binormal[3];
	float m_tangent[3];

	// цвет
	float m_color[4];
};

// Вершина для модели со скелетом
struct bqMDLMeshVertexSkinned
{
	// Базовые данные
	bqMDLMeshVertex m_base;

	// индексы костей
	uint8_t m_boneInds[4] = {0,0,0,0};

	// веса костей
	// в сумме не должно превышать 1.0
	// индекс массива соответсвует индексу массива 
	// в m_boneInds, то есть вес m_weights[0]
	// относится к кости, индекс которой указан в m_boneInds[0]
	float m_weights[4] = {0.f, 0.f, 0.f, 0.f};
};

// .mdl файл должен содержать всё необходимое для работы.
// Файл разбит на чанки - куски данных отвечающих за что-то.
// Каждый чанк содержит свой общий заголовок и заголовок
// конкретного типа чанка.
// 
// Начинается файла с главного заголовка.
struct bqMDLFileHeader
{
	// строка "bmdl"
	uint32_t m_bmld = 1818520930;
	
	// версия
	uint32_t m_version = 1;

	// количество чанков
	uint32_t m_chunkNum = 0;

	// типы алгоритмов сжатия. 
	enum
	{
		compression_null,

		// библиотека fastlz
		compression_fastlz,
	};
	// 0 - нет сжатия
	// 1 - сжато fastlz
	
	// Если есть сжатие то сжимается всё кроме bqMDLFileHeader
	uint32_t m_compression = compression_null;

	// если сжатие есть, размер не сжатых данных.
	uint32_t m_uncmpSz = 0;
	// если сжатие есть, размер сжатых данных.
	uint32_t m_cmpSz = 0;

	// 3D редактор имеет чуть иную систему координат.
	// Верх не Y а Z. Что-бы не мучится с ручным
	// преобразованием модели при экспорте, возни с
	// позициями костей и т.д., проще прочитать всё как
	// есть, и передать вращение. Потом в движке вращение
	// будет использовано в умножение на матрицу кости,
	// что приведёт к повороту модели, установки правильной
	// ориентации.
	// Не уверен на счёт моделей без кости. нужно проверить.
	// Вращение в радианах.
	float m_rotation[3] = { 0.f, 0.f, 0.f };
	
	// Можно изменить масштаб. Так-же применяется как
	// матрица, умножается на матрицу кости.
	float m_scale = 1.f;

	uint32_t m_reserved1 = 0;
	uint32_t m_reserved2 = 0;
	uint32_t m_reserved3 = 0;
	uint32_t m_reserved4 = 0;
};

// Общий заголовок для чанка
struct bqMDLChunkHeader
{
	// Тип
	enum
	{
		ChunkType__null,
		ChunkType_Mesh,
		ChunkType_String,
		ChunkType_Skeleton,
		ChunkType_Material,
		ChunkType_Animation,
		ChunkType_CollisionMesh,
	};
	uint32_t m_chunkType = ChunkType__null;
	
	// размер. bqMDLChunkHeader + header конкретного чанка 
	//    + размер данных этого чанка
	// Например для mesh chunk
	// m_chunkSz = sizeof(bqMDLChunkHeader) + sizeof(bqMDLChunkHeaderMesh) 
	// + meshHead.m_vertBufSz + meshHead.m_indBufSz;
	uint32_t m_chunkSz = 0;

	uint32_t m_reserved1 = 0;
	uint32_t m_reserved2 = 0;
};

struct bqMDLAABB
{
	//float m_aabbMin[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
	//float m_aabbMax[3] = { FLT_MIN, FLT_MIN, FLT_MIN };
	bqAabb m_aabb;


	void Add(float x, float y, float z)
	{
		m_aabb.Add(x, y, z);
		/*if (x > m_aabbMax[0]) m_aabbMax[0] = x;
		if (y > m_aabbMax[1]) m_aabbMax[1] = y;
		if (z > m_aabbMax[2]) m_aabbMax[2] = z;

		if (x < m_aabbMin[0]) m_aabbMin[0] = x;
		if (y < m_aabbMin[1]) m_aabbMin[1] = y;
		if (z < m_aabbMin[2]) m_aabbMin[2] = z;*/
	}

	/*bool SphereIntersect(const bqVec3& p, float r) const
	{
		bqReal dmin = 0.0;

		if (p.x < m_min.x) dmin -= sqrt(p.x - m_min.x);
		else if (p.x > m_max.x) dmin -= sqrt(p.x - m_max.x);

		if (p.y < m_min.y) dmin -= sqrt(p.y - m_min.y);
		else if (p.y > m_max.y) dmin -= sqrt(p.y - m_max.y);

		if (p.z < m_min.z) dmin -= sqrt(p.z - m_min.z);
		else if (p.z > m_max.z) dmin -= sqrt(p.z - m_max.z);

		return (dmin > 0.0);
	}*/
};

struct bqMDLBVHAABB
{
	//bqMDLAABB m_aabb;
	bqAabb m_aabb;

	uint32_t m_first = 0xffffffff;
	uint32_t m_second = 0xffffffff;
	uint32_t m_triNum = 0;
};

// дальше идёт зависимое от типа чанка (m_chunkType)

// Чанк описывающий 3D модель
struct bqMDLChunkHeaderMesh
{
	// все строки лежат отдельно.
	// здесь и далее указываются индексы строк
	uint32_t m_nameIndex = 0;
	
	bqMDLAABB m_aabb;

	// тип индекса
	enum
	{
		IndexType_16bit,
		IndexType_32bit,
	};
	uint32_t m_indexType = IndexType_16bit;
	
	// тип вершины
	enum
	{
		VertexType_Triangle,
		VertexType_TriangleSkinned,
	};
	uint32_t m_vertexType = VertexType_Triangle;
	
	// индекс строки имени материала
	uint32_t m_material = 0;

	// количество вершин
	uint32_t m_vertNum = 0;

	// количество индексов
	uint32_t m_indNum = 0;

	// размер вершинного буфера
	uint32_t m_vertBufSz = 0;

	// размер индексного буфера
	uint32_t m_indBufSz = 0;


	uint32_t m_reserved1 = 0;
	uint32_t m_reserved2 = 0;
};

struct bqMDLChunkHeaderCollisionMesh
{
	bqMDLAABB m_aabb;

	// количество вершин
	// vec3 float
	uint32_t m_vertNum = 0;

	// количество индексов
	// 32bit
	uint32_t m_indNum = 0;

	uint32_t m_numOfBVHAabbs = 0;
	uint32_t m_numOfBVHLeaves = 0;
};

// Чанк строки
struct bqMDLChunkHeaderString
{
	// размер строки
	// после m_strSz идёт строка без завершающего нуля
	uint32_t m_strSz = 0;
};


// Иерархия костей
// сразу после идут кости в соответствии с количеством (m_boneNum)
struct bqMDLChunkHeaderSkeleton
{
	// количество костей
	uint32_t m_boneNum = 0;
};
// 
struct bqMDLBoneData
{
	// индекс строки имени
	uint32_t m_nameIndex = 0;

	// индекс родителя
	// -1 значит что нет родителя
	int32_t m_parent = -1;

	// позиция
	float m_position[3];
	// масштаб
	float m_scale[3];
	// вращение
	float m_rotation[4]; // quaternion
};

// Материал
struct bqMDLChunkHeaderMaterial
{
	// индекс строки имени
	uint32_t m_nameIndex = 0;
	
	// индексы строк на имя файла:
	// diffuse map
	int32_t m_difMap = -1;
	// specular map
	int32_t m_specMap = -1;
	// normal map
	int32_t m_normMap = -1;
	// roughness map
	int32_t m_rougMap = -1;

	// параметры текстур
	enum
	{
		// 00000000 00000000 00000000 00000111
		texParams_cmpFunc_never = 0,
		texParams_cmpFunc_less,
		texParams_cmpFunc_equal,
		texParams_cmpFunc_lessEqual,
		texParams_cmpFunc_greater,
		texParams_cmpFunc_notEqual,
		texParams_cmpFunc_greaterEqual,
		texParams_cmpFunc_always,
		texParams_cmpFunc__mask_ = 0x7,

		// 00000000 00000000 00000000 00111000
		texParams_adrMode_wrap = 0,
		texParams_adrMode_mirror,
		texParams_adrMode_clamp,
		texParams_adrMode_border,
		texParams_adrMode_mirrorOnce,
		texParams_adrMode__mask_ = 0x38,

		// 00000000 00000000 00000111 11000000
		texParams_filter_PPP = 0,
		texParams_filter_PPL,
		texParams_filter_PLP,
		texParams_filter_PLL,
		texParams_filter_LPP,
		texParams_filter_LPL,
		texParams_filter_LLP,
		texParams_filter_LLL,
		texParams_filter_Anisotropic,
		texParams_filter_PPP_cmp,
		texParams_filter_PPL_cmp,
		texParams_filter_PLP_cmp,
		texParams_filter_PLL_cmp,
		texParams_filter_LPP_cmp,
		texParams_filter_LPL_cmp,
		texParams_filter_LLP_cmp,
		texParams_filter_LLL_cmp,
		texParams_filter_Anisotropic_cmp,
		texParams_filter__mask_ = 0x1C0,

		// 00000000 00000000 01111000 00000000
		texParams_anisotropicLevel1 = 1,
		texParams_anisotropicLevel2,
		texParams_anisotropicLevel3,
		texParams_anisotropicLevel4,
		texParams_anisotropicLevel5,
		texParams_anisotropicLevel6,
		texParams_anisotropicLevel7,
		texParams_anisotropicLevel8,
		texParams_anisotropicLevel9,
		texParams_anisotropicLevel10,
		texParams_anisotropicLevel__mask_ = 0x7800,
	};
	uint32_t m_textureParams = 0;

	// цвета
	float m_ambient[3] = { 0.5, 0.5, 0.5 };
	float m_diffuse[3] = { 1, 1, 1 };
	float m_specular[3] = { 1, 1, 1 };
};

// анимация
/* Выглядит типа так
* Имеем 4 кости
* 
* bqMDLChunkHeaderAnimation (m_framesNum=3)
* bqMDLAnimationData (frame0)
*     bqMDLAnimationData (bone0)
*     bqMDLAnimationData (bone1)
*     bqMDLAnimationData (bone2)
*     bqMDLAnimationData (bone3)
* bqMDLAnimationData (frame1)
*     bqMDLAnimationData (bone0)
*     bqMDLAnimationData (bone1)
*     bqMDLAnimationData (bone2)
*     bqMDLAnimationData (bone3)
* bqMDLAnimationData (frame2)
*     bqMDLAnimationData (bone0)
*     bqMDLAnimationData (bone1)
*     bqMDLAnimationData (bone2)
*     bqMDLAnimationData (bone3)
*/
struct bqMDLChunkHeaderAnimation
{
	// индекс строки имени
	uint32_t m_nameIndex = 0;

	// количество bqMDLAnimationData
	uint32_t m_framesNum = 0;
};

// позиция масштаб и вращение для анимации
struct bqMDLAnimationData
{
	float m_position[3];
	float m_scale[3];
	float m_rotation[4];
};

#endif

