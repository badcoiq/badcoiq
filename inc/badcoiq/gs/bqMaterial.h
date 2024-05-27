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
#ifndef __BQ_MATERIAL_H__
#define __BQ_MATERIAL_H__

#include "badcoiq/common/bqColor.h"
#include "badcoiq/containers/bqArray.h"
#include "badcoiq/gs/bqShader.h"

// Параметры для шейдеров
class bqMaterial
{
public:
	// для информации. шейдер устанавливается отдельным методом
	bqShaderType m_shaderType = bqShaderType::Standart;

	float m_opacity = 1.f;
	float m_alphaDiscard = 0.5f;
	bqColor m_colorDiffuse = bq::ColorWhite;
	bqColor m_colorAmbient = bq::ColorGray;
	bqColor m_colorSpecular = bq::ColorWhite;
	bqVec3 m_sunPosition = bqVec3(0.1, 0.7, 0.0);
	bool m_wireframe = false;
	bool m_cullBackFace = false;

	bqString m_name;

	// Надо переписать чтобы не засорять стек
	/*struct map
	{
		map()
		{
			for (int i = 0; i < 0x1000; ++i)
			{
				m_filePath[i] = 0;
			}
		}

		bqTexture* m_texture = 0;
		
		char8_t m_filePath[0x1000];

	}m_maps[16];*/

	/*struct map
	{
		map()
		{
			for (int i = 0; i < 0x1000; ++i)
			{
				m_filePath[i] = 0;
			}
		}

		bqTexture* m_texture = 0;

		char8_t m_filePath[0x1000];

	};
	map* m_maps = 0;*/

	struct map
	{
		map()
		{
			for (int i = 0; i < 0x1000; ++i)
			{
				m_filePath[i] = 0;
			}
		}

#if defined(BQ_WITH_IMAGE) && defined(BQ_WITH_GS)
		bqTexture* m_texture = 0;
#endif

		char8_t m_filePath[0x1000];
	};
	bqArray<map> m_maps;

	bqMaterial() 
	{
		//m_maps = (map*)bqMemory::malloc(sizeof(map) * 16);
		//for (uint32_t i = 0; i < 16; ++i)
		//{
		//	::new(&m_maps[i]) map(); // вызов конструктора
		//}
		for (uint32_t i = 0; i < 16; ++i)
		{
			m_maps.push_back(map());
		}
	}
	~bqMaterial()
	{
		//bqMemory::free(m_maps);
	}
};

#endif

