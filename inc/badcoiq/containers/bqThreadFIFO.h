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
#ifndef __bqThreadFIFO_H__
#define __bqThreadFIFO_H__

#include "badcoiq/containers/bqList.h"

#include <mutex>
template<typename Type, typename ContainerType = bqList<Type>>
class bqThreadFIFO
{
	ContainerType m_container;
	std::mutex m_mutex;
public:
	bqThreadFIFO() {}
	~bqThreadFIFO()
	{
		if (m_mutex.try_lock())
			m_mutex.unlock();
	}

	void Push(const Type& v)
	{
		m_mutex.lock();
		m_container.push_back(v);
		m_mutex.unlock();
	}

	void Pop()
	{
		m_mutex.lock();
		m_container.pop_front();
		m_mutex.unlock();
	}

	Type& Front()
	{
		m_mutex.lock();
		Type& r = m_container.front();
		m_mutex.unlock();
		return r;
	}
};


#endif

