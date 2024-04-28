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
#ifndef __bqFixedFIFO_H__
#define __bqFixedFIFO_H__

template<typename Type, uint32_t size>
class bqFixedFIFO
{
	uint32_t m_cur = 0;
	uint32_t m_insert = 0;
	uint32_t m_inUse = 0;
	Type m_data[size];
public:
	bqFixedFIFO() 
	{
	}
	
	~bqFixedFIFO()
	{
	}

	void Push(const Type& v)
	{
		m_data[m_insert] = v;
		
		if(m_inUse)
		{
			if(m_insert == m_cur)
			{
				++m_cur;
				if(m_cur >= size)
					m_cur = 0;
			}
		}
		else
		{
			m_cur = m_insert;
		}
		
		++m_inUse;
		if(m_inUse >= size)
			m_inUse = size;
		
		++m_insert;
		if(m_insert == size)
			m_insert = 0;
	}
	
	bool TryPush(const Type& v)
	{
		if (m_inUse != size)
		{
			Push(v);
			return true;
		}
		return false;
	}

	void Pop()
	{
		if (m_inUse)
		{
			--m_inUse;

			++m_cur;
			if (m_cur >= size)
				m_cur = 0;
		}
	}

	Type& Front()
	{
		Type& r = m_data[m_cur];
		return r;
	}

	bool IsEmpty() { return m_inUse == 0; }
	void Clear() { return m_inUse = 0; }
	bool IsFull() { return m_inUse == size; }
};


#endif

