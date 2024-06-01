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
#ifndef __BQ_BLOCKQUEUE_H__
#define __BQ_BLOCKQUEUE_H__

template<typename _Type>
class bqBlockQueue
{
	struct BlockNode
	{
		_Type* m_data = 0;
	};

	BlockNode* m_blocks = 0;
	uint32_t m_numOfBlocks = 0;
	uint32_t m_blockSize = 0;
	uint32_t m_putIndex = 0;
	bqBlockQueue() = delete;

public:
	bqBlockQueue(uint32_t blockSize, uint32_t numOfBlocks)
		:
		m_numOfBlocks(numOfBlocks),
		m_blockSize(blockSize)
	{
		m_blocks = (BlockNode*)bqMemory::malloc(m_numOfBlocks * sizeof(BlockNode));
		for (size_t i = 0; i < m_numOfBlocks; ++i)
		{
			m_blocks[i].m_data = (_Type*)bqMemory::malloc(m_blockSize * sizeof(_Type));
			for (size_t ei = 0; ei < m_blockSize; ++ei)
			{
				new(&m_blocks[i].m_data[ei]) _Type();
			}
		}
	}

	~bqBlockQueue()
	{
		for (size_t i = 0; i < m_numOfBlocks; ++i)
		{
			bqMemory::free(m_blocks[i].m_data);
		}
		bqMemory::free(m_blocks);
	}

	void Put(_Type* blockIn, _Type* blockOut)
	{
		// Сначала надо передать blockOut
		for (size_t i = 0; i < m_blockSize; ++i)
		{
			blockOut[i] = m_blocks[m_putIndex].m_data[i];

			m_blocks[m_putIndex].m_data[i] = blockIn[i];
		}

		++m_putIndex;

		if (m_putIndex == m_numOfBlocks)
			m_putIndex = 0;
	}

	uint32_t BlockSize()const { return m_blockSize; }

	_Type* GetCurrentData() { return m_blocks[m_putIndex].m_data; }
};


#endif

