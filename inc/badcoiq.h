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

/// \mainpage BADCOIQ Engine Documentation
/// 
/// Движок начинается с настройки компиляции \link badcoiq/bqConfig.h \endlink
/// 
/// Потом идут макросы.  \link badcoiq/common/bqDefines.h \endlink Есть такие макросы,
/// которые помогают процессу компиляции, есть такие,
/// которые помогают облегчить программирование
/// 
/// В badcoiq.h добавлены другие базовые include.
/// 
/// В \link badcoiq/common/bqUserData.h \endlink находится класс который можно наследовать.
/// Он даёт возможность установить и получить user data.
/// Иногда бывает необходимым подобный функционал. Достаточно просто наследовать этот класс.
/// 
/// В \link badcoiq/common/bqMemory.h \endlink находится класс который можно наследовать.
///

#pragma once
#ifndef __BQ_BADCOIQ_H__
/// \cond
#define __BQ_BADCOIQ_H__
/// \endcond

// если компилировать со всеми warnings и считать их ошибками
// то решение некоторых warnings ухудшают код
// некоторые выдаются внутри кода visual studio
#pragma warning( disable : 4820 ) // <число> padding что-то там. например если добавлен bool в класс
#pragma warning( disable : 4365 ) // выдаёт xmemory. conversion from 'long' to 'unsigned int', signed/unsigned mismatch
#pragma warning( disable : 4244 ) // double to float, possible loss of data

#include "badcoiq/bqConfig.h"
#include "badcoiq/common/bqDefines.h"
#include "badcoiq/common/bqUserData.h"
#include "badcoiq/common/bqMemory.h"
#include "badcoiq/common/bqPtr.h"
#include "badcoiq/common/bqLog.h"
#include "badcoiq/common/bqForward.h"
#include "badcoiq/string/bqString.h"
#include "badcoiq/system/bqStacktracer.h"
#include "badcoiq/common/bqAssert.h"
#include "badcoiq/system/bqDLL.h"
#include "badcoiq/common/bqBasicTypes.h"
#include "badcoiq/common/bqHierarchy.h"
#include "badcoiq/framework/bqFramework.h"

#endif

