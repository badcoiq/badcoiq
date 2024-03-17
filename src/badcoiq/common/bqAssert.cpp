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

#include "badcoiq.h"

#ifdef BQ_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

// Дефолтная функция для assert
// Надо раздокументировать когда будут готовы log, stacktracer и string
void bqOnAssert_default(const char* message, const char* file, uint32_t line, uint32_t flags)
{
#ifdef BQ_DEBUG
    if ((flags & bq::internal::flag_onAssert_useStackTrace) == bq::internal::flag_onAssert_useStackTrace)
    {
        bqStackTracer::Print();
    }

    if ((flags & bq::internal::flag_onAssert_useDefaultSystemWindow) == bq::internal::flag_onAssert_useDefaultSystemWindow)
    {
        bqString str_msg(message);
        bqString str_file(file);

        bqStringW str1;
        str_msg.to_utf16(str1);

        bqStringW str2;
        str_file.to_utf16(str2);

#ifdef BQ_PLATFORM_WINDOWS
        _wassert((const wchar_t*)str1.data(), (const wchar_t*)str2.data(), line);
#endif
    }
    else
    {
        //printf("%s %s %i\n", message, file, line);
       bqLog::PrintError("%s %s %i\n", message, file, line);
#ifdef BQ_PLATFORM_WINDOWS
        DebugBreak();
#endif
    }
#endif
}

// Указатель на функцию. Можно будет изменять функцию OnAssert используя функцию SetOnAssert
static void(*g_onAssert)(const char* message, const char* file, uint32_t line, uint32_t flags) = bqOnAssert_default;

namespace bq
{
    namespace internal
    {
        // просто вызываем g_onAssert
        void BQ_CDECL OnAssert(const char* message, const char* file, uint32_t line, uint32_t flags)
        {
            g_onAssert(message, file, line, flags);
        }
    
        // Изменяем g_onAssert. Или устанавливаем дефолтную функцию.
        void BQ_CDECL SetOnAssert(void(*f)(const char* message, const char* file, uint32_t line, uint32_t flags))
        {
            g_onAssert = f ? f : bqOnAssert_default;
        }
    }
}

