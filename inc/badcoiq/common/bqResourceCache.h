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
#ifndef __BQ_RESOURCECACHE_H__
/// \cond
#define __BQ_RESOURCECACHE_H__
/// \endcond

#include "badcoiq/cryptography/bqCryptography.h"

/// Контейнер для ресурса
template<class _type>
struct bqResourceCacheNode
{
	_type m_resource;
	bqMD5 m_md5;

	/// чтобы заного загрузить текстуру нужно знать где
	/// лежит файл
	bqStringA* m_path = 0;
};

/// \brief Базовый класс для создания контейнеров хранящих
/// какой-то ресурс.
template<class _type>
class bqResourceCache
{
	bqArray<bqResourceCacheNode<_type>*> m_data;
public:
	bqResourceCache() { m_data.reserve(100); }
	virtual ~bqResourceCache() { Clear(); }
	BQ_PLACEMENT_ALLOCATOR(bqResourceCache);

	virtual void Free(_type r) = 0;
	virtual _type Load(const char* path) = 0;

	_type Get(const char* path)
	{
		_type t = Get(bqCryptography::MD5(path, strlen(path)));
		if (!t)
			t = Get(path, 0, true);
		return t;
	}
	_type Get(const bqMD5& md5)
	{
		for (size_t i = 0; i < m_data.m_size; ++i)
		{
			if (bqCryptography::Compare(m_data.m_data[i]->m_md5, md5))
				return m_data.m_data[i]->m_resource;
		}
		return 0;
	}
	_type Get(const char* path, uint32_t* outIndex, bool load = true)
	{
		_type t = Get(bqCryptography::MD5(path, strlen(path)));
		if (!t)
		{
			bqResourceCacheNode<_type>* n = new bqResourceCacheNode<_type>;
			n->m_resource = 0;
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
	_type Get(uint32_t i)
	{
		if (i < m_data.m_size)
			return m_data.m_data[i]->m_resource;
		return 0;
	}
	uint32_t GetNum()
	{
		return (uint32_t)m_data.m_size;
	}
	void Clear()
	{
		for (size_t i = 0; i < m_data.m_size; ++i)
		{
			Free(m_data.m_data[i]->m_resource);
			m_data.m_data[i]->m_resource = 0;

			if (m_data.m_data[i]->m_path)
				delete m_data.m_data[i]->m_path;
			delete m_data.m_data[i];
		}
		m_data.clear();
	}
	uint32_t GetIndex(const char* path)
	{
		return GetIndex(bqCryptography::MD5(path, strlen(path)));
	}
	uint32_t GetIndex(const bqMD5& md5)
	{
		for (size_t i = 0; i < m_data.m_size; ++i)
		{
			if (bqCryptography::Compare(m_data.m_data[i]->m_md5, md5))
				return (uint32_t)i;
		}
		return 0xFFFFFFFF;
	}
	uint32_t GetIndex(_type r)
	{
		for (size_t i = 0; i < m_data.m_size; ++i)
		{
			if (m_data.m_data[i]->m_resource == r)
				return (uint32_t)i;
		}
		return 0xFFFFFFFF;
	}

	void Unload(uint32_t i)
	{
		if (i < m_data.m_size)
		{
			Free(m_data.m_data[i]->m_resource);
			m_data.m_data[i]->m_resource = 0;
		}
	}


	_type Reload(uint32_t index, bool forceUnload = false)
	{
		_type t = 0;
		if (index < m_data.m_size)
		{
			if (forceUnload)
				Unload(index);

			if (m_data.m_data[index]->m_path
				&& !m_data.m_data[index]->m_resource)
			{
				t = Load(m_data.m_data[index]->m_path->c_str());
				m_data.m_data[index]->m_resource = t;
			}

		}
		return t;
	}
	bool IsLoaded(uint32_t i)
	{
		if (i < m_data.m_size)
			return (m_data.m_data[i]->m_resource != 0);
		return false;
	}
};

#endif

