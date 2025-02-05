/*
BSD 2-Clause License

Copyright (c) 2025, badcoiq
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
#ifndef __BQ_DLL_H__
#define __BQ_DLL_H__

/// надо завести свои имена для хэндла и указателя на функцию
typedef void* bqDLLHandle;   // HMODULE в Windows
typedef void* bqDLLFunction;

/// \brief Подобно bqMemory, это класс со статическими методами.
class bqDLL
{
public:

	/// Загрузить динамическую библиотеку
	/// Вернётся указатель если загрузилось.
	/// При завершении работы надо вызвать free и передать bqDLLHandle
	static bqDLLHandle Load(const char* libraryName);

	/// Выгрузить библиотеку
	static void Free(bqDLLHandle library);

	/// Получить функцию из библиотеки
	/// Вернёт NULL если была какая-то ошибка.
	static bqDLLFunction GetProc(bqDLLHandle library, const char* functionName);
};

#endif

