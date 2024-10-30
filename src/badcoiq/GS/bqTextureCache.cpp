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
#include "badcoiq/gs/bqTexture.h"

#ifdef BQ_WITH_GS

#include "badcoiq/gs/bqGS.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqTextureCache::bqTextureCache(bqGS* gs)
	:
	m_gs(gs)
{
	m_data.reserve(100);
}

bqTextureCache::~bqTextureCache()
{
	Clear();
}

bqTexture* bqTextureCache::GetTexture(const bqMD5& md5)
{
	for (size_t i = 0; i < m_data.m_size; ++i)
	{
		if (bqCryptography::Compare(m_data.m_data[i]->m_md5, md5))
		{
			return m_data.m_data[i]->m_texture;
		}
	}
	return 0;
}

bqTexture* bqTextureCache::GetTexture(const char* path)
{
	bqTexture* t = GetTexture(bqCryptography::MD5(path, strlen(path)));

	if (!t)
		t = GetTexture(path, 0, true);
	return t;
}

bqTexture* bqTextureCache::GetTexture(const char* path, uint32_t* outIndex, bool load)
{
	bqTexture* t = GetTexture(bqCryptography::MD5(path, strlen(path)));
	if (!t)
	{
		_node* n = new _node;
		n->m_texture = 0;
		n->m_md5 = bqCryptography::MD5(path, strlen(path));
		n->m_path = new bqStringA(path);
		m_data.push_back(n);

		uint32_t index = m_data.size() - 1;

		if (outIndex)
			*outIndex = index;

		if (load)
			t = Reload(index);
	}
	return t;
}

/// Удалить все текстуры (освободить память)
void bqTextureCache::Clear()
{
	for (size_t i = 0; i < m_data.m_size; ++i)
	{
		if (m_data.m_data[i]->m_texture)
			delete m_data.m_data[i]->m_texture;
		if (m_data.m_data[i]->m_path)
			delete m_data.m_data[i]->m_path;
		delete m_data.m_data[i];
	}
	m_data.clear();
}

/// Получить количество текстур в кеше
uint32_t bqTextureCache::GetTextureNum()
{
	return (uint32_t)m_data.m_size;
}

/// Получить текстуру по индексу
bqTexture* bqTextureCache::GetTexture(uint32_t i)
{
	if(i < m_data.m_size)
		return m_data.m_data[i]->m_texture;
	return 0;
}

void bqTextureCache::Unload(uint32_t i)
{
	if (i < m_data.m_size)
	{
		if (m_data.m_data[i]->m_texture)
		{
			delete m_data.m_data[i]->m_texture;
			m_data.m_data[i]->m_texture = 0;
		}
	}
}

bqTexture* bqTextureCache::Reload(uint32_t index, bool forceUnload)
{
	bqTexture* t = 0;
	if (index < m_data.m_size)
	{
		if (forceUnload)
			Unload(index);

		if (m_data.m_data[index]->m_path
			&& !m_data.m_data[index]->m_texture)
		{
			bqImage* img = bqFramework::SummonImage(m_data.m_data[index]->m_path->c_str());
			if (img)
			{
				m_data.m_data[index]->m_texture = m_gs->SummonTexture(img, m_textureInfo);
				delete img;

				if (m_data.m_data[index]->m_texture)
					t = m_data.m_data[index]->m_texture;
			}
		}
	}
	return t;
}

bool bqTextureCache::IsLoaded(uint32_t i)
{
	if (i < m_data.m_size)
		return (m_data.m_data[i]->m_texture != 0);
	return false;
}


#endif
