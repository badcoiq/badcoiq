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
#ifndef __BQ_LOG_H__
#define __BQ_LOG_H__

// Класс для вывода текстового сообщения
// Можно установить коллбэки
class bqLog
{
public:

	// Просто что-то написать. Используется сишная функция vsnprintf
	static void Print(const char* s, ...);
	static void Print(const wchar_t* s, ...);

	// Тоже что Print, только сначала напишет "Info"
	static void PrintInfo(const char* s, ...);
	static void PrintInfo(const wchar_t* s, ...);

	// Тоже что Print, только сначала напишет "Warning"
	static void PrintWarning(const char* s, ...);
	static void PrintWarning(const wchar_t* s, ...);

	// Тоже что Print, только сначала напишет "Error"
	static void PrintError(const char* s, ...);
	static void PrintError(const wchar_t* s, ...);

	// Установить свой коллбэк
	// Если послать 0 то будет установлен дефолтный коллбэк
	static void SetCallbackA(void(*)(const char*));
	static void SetCallbackW(void(*)(const wchar_t*));

	static void SetInfoWord(const char*);
};


#endif

