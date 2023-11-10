/*
BSD 2-Clause License

Copyright (c) 2023, badcoiq
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
#ifndef __BQ_QUEUE_H__
#define __BQ_QUEUE_H__

#include "badcoiq/common/bqPoolAllocator.h"

template<typename Type>
class bqQueue
{
	struct _Node
	{
		Type m_data;
		_Node* m_next = 0;
	};
	_Node* m_head = 0;
	_Node* m_tail = 0;

	bqPoolAllocator* m_memory = 0;

	bqQueue(const bqQueue&) = delete;
	bqQueue(bqQueue&&) = delete;
	bqQueue& operator=(const bqQueue&) = delete;
	bqQueue& operator=(bqQueue&&) = delete;

public:
	typedef Type* pointer;
	typedef Type& reference;
	typedef const Type& const_reference;
	using value_type = Type;

	bqQueue(uint32_t size)
	{
		BQ_ASSERT_ST(size);
		m_memory = bqCreate<bqPoolAllocator>(sizeof(_Node), size);
	}
	
	~bqQueue()
	{
		bqDestroy(m_memory);
	}

	reference Front()
	{
		BQ_ASSERT_ST(m_head);
		return m_head->m_data;
	}

	const_reference Front() const
	{
		BQ_ASSERT_ST(m_head);
		return m_head->m_data;
	}

	reference Back()
	{
		BQ_ASSERT_ST(m_head);
		return m_tail->m_data;
	}

	const_reference Back() const
	{
		BQ_ASSERT_ST(m_head);
		return m_tail->m_data;
	}

	uint32_t Size()
	{
		return m_memory->GetMaxCount() - m_memory->GetFreeCount();
	}

	void Push(const Type& value)
	{
		if (m_memory->GetFreeCount())
		{
			_Node* newNode = (_Node*)m_memory->Allocate(1);
			::new(newNode) _Node();

			newNode->m_data = value;

			if (m_head)
			{
				m_tail->m_next = newNode;
				m_tail = newNode;
			}
			else
			{
				m_head = newNode;
				m_tail = newNode;
			}
		}
	}

	void Pop()
	{
		if (Size())
		{
			if (m_head)
			{
				_Node* popnode = m_head;

				m_memory->FreeMemory(popnode);

				m_head = m_head->m_next;

				if (popnode == m_head)
				{
					m_head = 0;
					m_tail = 0;
				}
			}
		}
	}
};

#endif

