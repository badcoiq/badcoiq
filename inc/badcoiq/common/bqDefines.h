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
#ifndef __BQ_DEFINES_H__
/// \cond
#define __BQ_DEFINES_H__
/// \endcond

/// \file bqDefines.h
/// \brief Макросы и стандартные инклюды

#include <stddef.h> // NULL, size_t
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h> 
#include <time.h>   // clock_t
#include <stdint.h> // int32_t uint32_t и другие
#include <ctype.h>  // isspace
#include <utility>  // std::move

/// Версия цифрами
#define BQ_VERSION_MAJOR 0
#define BQ_VERSION_MINOR 1
#define BQ_VERSION_PATCH 0

#ifdef _MSC_VER
/// Вместо inline теперь надо писать BQ_FORCEINLINE
#define BQ_FORCEINLINE __forceinline
#else
/// Вместо inline теперь надо писать BQ_FORCEINLINE
#define BQ_FORCEINLINE inline
#endif

/// Для координат в 3D, матриц и  т.д. надо дать возможность указать что использовать, double или float.
/// Такому типу обычно дают имя `real`
using bqReal = double;

using bqFloat32 = float;
using bqFloat64 = double;

using float32_t = bqFloat32;
using float64_t = bqFloat64;

#if defined(WIN32) | defined(_WIN64) | defined(_WIN32)
/// Определяем платформу
#define BQ_PLATFORM_WINDOWS
#else
#error Только для Windows
#endif

#ifdef BQ_PLATFORM_WINDOWS
/// Для использования Си (функций из.c файлов) внутри C++
#define BQ_CDECL _cdecl
#else
#error Только для Windows
#endif

#ifdef _DEBUG
/// Свой макрос для дебага
#define BQ_DEBUG
#endif

/// Упаковка 4х байтов в один uint32_t
/// uint32_t magic = BQ_MAKEFOURCC('‰', 'P', 'N', 'G');
#define BQ_MAKEFOURCC( ch0, ch1, ch2, ch3 )\
	((uint32_t)(uint8_t)(ch0)|((uint32_t)(uint8_t)(ch1)<<8)|\
	((uint32_t)(uint8_t)(ch2)<<16)|((uint32_t)(uint8_t)(ch3)<<24))

#ifdef BQ_PLATFORM_WINDOWS
#if defined _WIN64 || defined __x86_64__
/// Надо узнать, является ли конфигурация 64бит
#define BQ_BIT_64
#endif
#else
#error Только для Windows
#endif

/// Для показа имени файла, строки и функции
/// printf("%s %i %s\n", BQ_FILE, BQ_LINE, BQ_FUNCTION);
#ifdef BQ_PLATFORM_WINDOWS
#define BQ_FILE __FILE__
#define BQ_LINE __LINE__
#define BQ_FUNCTION __FUNCTION__
#else
#error Только для Windows
#endif

#ifdef _MSC_VER

/// Используемы Visual Studio
/// \cond
#if _MSC_VER >= 1930 && _MSC_VER <= 1937
/// \endcond
#define BQ_LINK_LIBRARY_CMP "_v143"
#elif _MSC_VER >= 1920 && _MSC_VER <= 1929
#define BQ_LINK_LIBRARY_CMP "_v142"
#elif _MSC_VER >= 1910 && _MSC_VER <= 1916
#error Не проверено.
#define BQ_LINK_LIBRARY_CMP "_v141"
#else
#error Need to update this section
#endif

/// 32 или 64 бит конфигурация
#ifdef BQ_BIT_64
#define BQ_LINK_LIBRARY_ARCH "_x64"
#else
#define BQ_LINK_LIBRARY_ARCH "_x86"
#endif

// Debug или Release
#ifdef BQ_DEBUG
#define BQ_LINK_LIBRARY_CONF "_Debug"
#else
#define BQ_LINK_LIBRARY_CONF "_Release"
#endif

/// Использование : BQ_LINK_LIBRARY("libname")
#define BQ_LINK_LIBRARY(n) \
	__pragma(comment(lib, n BQ_LINK_LIBRARY_CMP BQ_LINK_LIBRARY_ARCH BQ_LINK_LIBRARY_CONF ".lib"))

#endif

/// Полезная вещь.Например имеем 0xAABBCCDD
/// Можно получить 0xAABB и 0xCCDD
#define BQ_LO32(l) ((uint16_t)(((uint32_t)(l)) & 0xffff))
#define BQ_HI32(l) ((uint16_t)((((uint32_t)(l)) >> 16) & 0xffff))

/// вызывать только если объект был создан с помощью new
/// в том числе если у класса указано BQ_PLACEMENT_ALLOCATOR
#define BQ_SAFEDESTROY(p) if(p){delete p;p=nullptr;}
#define BQ_SAFEDESTROY_A(p) if(p){delete[] p;p=nullptr;}

/// наверно надо удалить
#define BQ_DELETED_METHODS(x) x(const x&) = delete; \
	x& operator=(const x&) = delete;
	
/// сравнение класса-строки с простой строкой
#define BQ_CMPSTRA(s1,s2) (strcmp(s1.c_str(), s2) == 0)

#endif

