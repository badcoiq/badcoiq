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
#ifndef __bqLIFO_H__
#define __bqLIFO_H__


//#include <mutex>

template<typename Type, uint32_t size>
class bqLIFO
{
	uint32_t m_sz = 0;
	Type m_data[size];
public:
	bqLIFO() 
	{
	}
	
	~bqLIFO()
	{
	}

	void Push(const Type& v)
	{
		if (m_sz < size)
		{
			m_data[m_sz] = v;
			++m_sz;
		}
	}
	
	void Pop()
	{
		if (m_sz)
			--m_sz;
	}

	void Get(Type* out)
	{
		if (m_sz)
			*out = m_data[m_sz - 1];
		else
			*out = 0;
	}

	bool IsEmpty() { return m_sz == 0; }
	void Clear() { m_sz = 0; }
	bool IsFull() { return m_sz == size-1; }
};

#endif

