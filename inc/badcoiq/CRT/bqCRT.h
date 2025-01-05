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
#ifndef __BQ_CRT_H__
/// \cond
#define __BQ_CRT_H__
/// \endcond

/// \brief Набор методов как в C Run Time library
///
/// С дополненным функционалом (иначе бы не делал).
class bqCRT
{
public:
	static int atoi(const char*);
	static int atoi(const wchar_t*);
	static int atoi(const char32_t*);
	static size_t strlen(const char*);
	static size_t strlen(const wchar_t*);
	static size_t strlen(const char32_t*);

	static int fprintf(bqStream* stream, const char* format, ...);
	static int fprintf(bqStream* stream, const wchar_t* format, ...);
	static int fprintf(bqStream* stream, const char32_t* format, ...);

	static int vfprintf(bqStream* stream, const char* format, va_list arg);
	static int vfprintf(bqStream* stream, const wchar_t* format, va_list arg);
	static int vfprintf(bqStream* stream, const char32_t* format, va_list arg);

	static int fputc(char character, bqStream* stream);
	static int fputc(wchar_t character, bqStream* stream);
	static int fputc(char32_t character, bqStream* stream);

	static int strcmp(const char* str, int limit, const char* other);
	static int strcmp(const wchar_t* str, int limit, const wchar_t* other);
	static int strcmp(const char32_t* str, int limit, const char32_t* other);

	static int islower(char c);
	static int islower(wchar_t c);
	static int islower(char32_t c);
	static int isupper(char c);
	static int isupper(wchar_t c);
	static int isupper(char32_t c);
	static char tolower(char c);
	static wchar_t tolower(wchar_t c);
	static char32_t tolower(char32_t c);
	static char toupper(char c);
	static wchar_t toupper(wchar_t c);
	static char32_t toupper(char32_t c);

	//	static char putchar(char, const char* buffer, size_t bufferSz);
	//	static wchar_t putchar(wchar_t, const char* buffer, size_t bufferSz);
	//	static char32_t putchar(char32_t, const char* buffer, size_t bufferSz);
};

#endif

