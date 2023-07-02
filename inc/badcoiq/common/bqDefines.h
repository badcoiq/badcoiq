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
#ifndef __BQ_DEFINES_H__
#define __BQ_DEFINES_H__

#include <stddef.h> // NULL, size_t
#include <stdlib.h>
#include <stdio.h>
#include <assert.h> 
#include <stdint.h> // int32_t uint32_t и другие
#include <ctype.h>  // isspace
#include <utility>  // std::move

// Возможно ненужная вещь. Пережиток прошлого и всё такое.
// Многие библиотеки используют это.
// `inline` не гарантирует что функция будет inline.
// Предположу что "не гарантирует" если функция написана в .c .cpp файлах
//   (или файл инклюдится в них), ведь такие функции не встраиваемые, по умолчанию.
// Вместо inline теперь надо писать BQ_FORCEINLINE
#ifdef _MSC_VER
#define BQ_FORCEINLINE __forceinline
#else
#define BQ_FORCEINLINE inline
#endif

// Для координат в 3D, матриц и  т.д. надо дать возможность указать что использовать, double или float.
// Такому типу обычно дают имя `real`
#define bqReal double

// Определяем платформу
#if defined(WIN32) | defined(_WIN64) | defined(_WIN32)
#define BQ_PLATFORM_WINDOWS
#else
#error Только для Windows
#endif

// Для использования Си (функций из.c файлов) внутри C++
#ifdef BQ_PLATFORM_WINDOWS
#define BQ_CDECL _cdecl
#else
#error Только для Windows
#endif

// Свой макрос для дебага
#ifdef _DEBUG
#define BQ_DEBUG
#endif

// Упаковка 4х байтов в один uint32_t
// uint32_t magic = BQ_MAKEFOURCC('‰', 'P', 'N', 'G');
#define BQ_MAKEFOURCC( ch0, ch1, ch2, ch3 )\
	((uint32_t)(uint8_t)(ch0)|((uint32_t)(uint8_t)(ch1)<<8)|\
	((uint32_t)(uint8_t)(ch2)<<16)|((uint32_t)(uint8_t)(ch3)<<24))

// Надо узнать, является ли конфигурация 64бит
#ifdef BQ_PLATFORM_WINDOWS
#if defined _WIN64 || defined __x86_64__
#define BQ_BIT_64
#endif
#else
#error Только для Windows
#endif

// Для показа имени файла, строки и функции
// printf("%s %i %s\n", BQ_FILE, BQ_LINE, BQ_FUNCTION);
#ifdef BQ_PLATFORM_WINDOWS
#define BQ_FILE __FILE__
#define BQ_LINE __LINE__
#define BQ_FUNCTION __FUNCTION__
#else
#error Только для Windows
#endif

#ifdef _MSC_VER

#if _MSC_VER >= 1930 && _MSC_VER <= 1936
#define BQ_LINK_LIBRARY_CMP "_v143"
#elif _MSC_VER >= 1920 && _MSC_VER <= 1929
#define BQ_LINK_LIBRARY_CMP "_v142"
#elif _MSC_VER >= 1910 && _MSC_VER <= 1916
#error Не проверено.
#define BQ_LINK_LIBRARY_CMP "_v141"
#endif

#ifdef BQ_BIT_64
#define BQ_LINK_LIBRARY_ARCH "_x64"
#else
#define BQ_LINK_LIBRARY_ARCH "_x86"
#endif

#ifdef BQ_DEBUG
#define BQ_LINK_LIBRARY_CONF "_Debug"
#else
#define BQ_LINK_LIBRARY_CONF "_Release"
#endif

// Использование : BQ_LINK_LIBRARY("libname")
#define BQ_LINK_LIBRARY(n) \
	__pragma(comment(lib, n BQ_LINK_LIBRARY_CMP BQ_LINK_LIBRARY_ARCH BQ_LINK_LIBRARY_CONF ".lib"))

#endif

// Полезная вещь.Например имеем 0xAABBCCDD
// Можно получить 0xAABB и 0xCCDD
#define BQ_LO32(l) ((uint16_t)(((uint32_t)(l)) & 0xffff))
#define BQ_HI32(l) ((uint16_t)((((uint32_t)(l)) >> 16) & 0xffff))

#endif

