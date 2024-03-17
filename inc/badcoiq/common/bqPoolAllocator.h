/*
Copyright (c) 2003-2013 Gino van den Bergen / Erwin Coumans  http://bulletphysics.org
Copyright (c) 2024, badcoiq

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#pragma once
#ifndef __BQ_POOLALLOC_H__
#define __BQ_POOLALLOC_H__

class bqPoolAllocator
{
	uint32_t m_elemSize;
	uint32_t m_maxElements;
	uint32_t m_freeCount;
	void* m_firstFree;
	uint8_t* m_pool;

public:
	bqPoolAllocator(uint32_t elemSize, uint32_t maxElements)
		: m_elemSize(elemSize),
		m_maxElements(maxElements)
	{
		BQ_ASSERT_ST(elemSize);
		BQ_ASSERT_ST(maxElements);

		m_pool = (uint8_t*)bqMemory::malloc(m_elemSize * m_maxElements);

		uint8_t* p = m_pool;
		m_firstFree = p;
		m_freeCount = m_maxElements;
		uint32_t count = m_maxElements;
		while (--count)
		{
			*(void**)p = (p + m_elemSize);
			p += m_elemSize;
		}
		*(void**)p = 0;
	}

	~bqPoolAllocator()
	{
		bqMemory::free(m_pool);
	}

	uint32_t GetFreeCount() const
	{
		return m_freeCount;
	}

	uint32_t GetUsedCount() const
	{
		return m_maxElements - m_freeCount;
	}

	uint32_t GetMaxCount() const
	{
		return m_maxElements;
	}

	void* Allocate(uint32_t size)
	{
		// release mode fix
		(void)size;
		BQ_ASSERT_ST(!size || size <= m_elemSize);
		BQ_ASSERT_ST(m_freeCount > 0);
		void* result = m_firstFree;
		m_firstFree = *(void**)m_firstFree;
		--m_freeCount;
		return result;
	}

	bool ValidPtr(void* ptr)
	{
		if (ptr)
		{
			if (((uint8_t*)ptr >= m_pool && (uint8_t*)ptr < m_pool + m_maxElements * m_elemSize))
			{
				return true;
			}
		}
		return false;
	}

	void FreeMemory(void* ptr)
	{
		if (ptr)
		{
			BQ_ASSERT_ST((uint8_t*)ptr >= m_pool && (uint8_t*)ptr < m_pool + m_maxElements * m_elemSize);

			*(void**)ptr = m_firstFree;
			m_firstFree = ptr;
			++m_freeCount;
		}
	}

	int GetElementSize() const
	{
		return m_elemSize;
	}

	uint8_t* GetPoolAddress()
	{
		return m_pool;
	}

	const uint8_t* GetPoolAddress() const
	{
		return m_pool;
	}
};

#endif

