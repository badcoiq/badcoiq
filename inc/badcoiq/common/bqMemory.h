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
#ifndef __BQ_MEMORY_H__
#define __BQ_MEMORY_H__

// Использовать эти функции вместо malloc\free new\delete
class bqMemory
{
public:
	static void* malloc(size_t size);
	static void* calloc(size_t size);
	static void  free(void*);
	static void* realloc(void*, size_t size);
};

// Данные функции неудобно использовать для замены new и delete
// Потому что для объектов классов нужно вызывать конструктор и деструктор.
// Добавлю две функции которые будут выделять\освобождать память и вызывать конструктор\деструктор.
template<typename _type, typename... Args>
_type* bqCreate(Args&&... args)
{
	_type* ptr = (_type*)bqMemory::malloc(sizeof(_type));
	if (ptr)
		::new(ptr) _type(std::forward<Args>(args)...);
	return ptr;
}

template<typename _type>
void bqDestroy(_type* ptr)
{
	assert(ptr);
	ptr->~_type();
	bqMemory::free(ptr);
}
// Для шаблонных функций force inline не нужен, так как они все получаются встраиваемыми.

// Чтобы было удобнее создавать объекты классов.
// Класс должен иметь BQ_PLACEMENT_ALLOCATOR(имя класса);
#define BQ_PLACEMENT_ALLOCATOR(c) void* operator new(std::size_t count){ return (c*)bqMemory::malloc(count);}\
void operator delete(void* ptr){if (ptr) bqDestroy(ptr);}


#endif

