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
#ifndef __BQ_PTR_H__
#define __BQ_PTR_H__

template<typename Type>
class bqPtr
{
	using delete_method_type = void(bqPtr::*)();
	delete_method_type m_delete = 0;

	Type* m_ptr = 0;

	void _deleteFree()
	{
		bqMemory::free(m_ptr);
	}

	void _deleteDestroy()
	{
		bqDestroy(m_ptr);
	}

public:

	bqPtr(Type* p, delete_method_type delete_method)
		:
		m_ptr(p)
	{
		m_delete = delete_method;
	}

	~bqPtr()
	{
		if (m_ptr)
			(this->*m_delete)();
	}

	static delete_method_type Free()
	{
		return &bqPtr::_deleteFree;
	}

	static delete_method_type Destroy()
	{
		return &bqPtr::_deleteDestroy;
	}

	Type* Ptr() { return m_ptr; }

	Type* Drop() { Type* p = m_ptr; m_ptr = 0; return p; }

	Type* operator->() const { return m_ptr; }
};

// для ускорения создания bqPtr
#define BQ_PTR_D(t,n,x) bqPtr<t> (n)((t*)(x), bqPtr<t>::Destroy());
#define BQ_PTR_F(t,n,x) bqPtr<t> (n)((t*)(x), bqPtr<t>::Free());

#endif

