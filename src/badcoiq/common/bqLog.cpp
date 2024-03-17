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

#include <stdarg.h>

// Дефолтные колбэки
void bqLogDefaultCallbackA(const char* s)
{
	printf(s);
}

void bqLogDefaultCallbackW(const wchar_t* s)
{
	wprintf(s);
}

// класс который будет всё выполнять
class bqLogImpl
{
public:
	bqLogImpl()
	{
		m_cba = bqLogDefaultCallbackA;
		m_cbw = bqLogDefaultCallbackW;
	}

	void vprintf(const char* s, va_list vl);
	void vwprintf(const wchar_t* s, va_list vl);

	void(*m_cba)(const char*);
	void(*m_cbw)(const wchar_t*);
};

void bqLogImpl::vprintf(const char* s, va_list vl)
{
	char buffer[0xFFFF];
	vsnprintf_s(buffer, 0xFFFF, s, vl);  // заполняем буфер
	m_cba(buffer); // вызываем коллбэк
}

void bqLogImpl::vwprintf(const wchar_t* s, va_list vl)
{
	wchar_t buffer[0xFFFF];
	vswprintf_s(buffer, 0xFFFF, s, vl);
	m_cbw(buffer);
}

static bqLogImpl g_log;

void bqLog::Print(const char* s, ...)
{
	va_list vl;
	va_start(vl, s); 
	g_log.vprintf(s, vl); // получаем аргументы и посылаем в g_log
	va_end(vl);
}

void bqLog::Print(const wchar_t* s, ...)
{
	va_list vl;
	va_start(vl, s);
	g_log.vwprintf(s, vl);
	va_end(vl);
}

static const char* g_bqLogInfoWordDefault = "Info: ";
const char* g_bqLogInfoWord = g_bqLogInfoWordDefault;

void bqLog::SetInfoWord(const char* s)
{
	if (s)
		g_bqLogInfoWord = s;
	else
		g_bqLogInfoWord = g_bqLogInfoWordDefault;


}

void bqLog::PrintInfo(const char* s, ...)
{
	Print("%s", g_bqLogInfoWord);

	va_list vl;
	va_start(vl, s);
	g_log.vprintf(s, vl);
	va_end(vl);
}

void bqLog::PrintInfo(const wchar_t* s, ...)
{
	Print("%s", g_bqLogInfoWord);
	va_list vl;
	va_start(vl, s);
	g_log.vwprintf(s, vl);
	va_end(vl);
}

void bqLog::PrintWarning(const char* s, ...)
{
	Print("%s", "Warning: ");
	va_list vl;
	va_start(vl, s);
	g_log.vprintf(s, vl);
	va_end(vl);
}

void bqLog::PrintWarning(const wchar_t* s, ...)
{
	Print("%s", "Warning: ");
	va_list vl;
	va_start(vl, s);
	g_log.vwprintf(s, vl);
	va_end(vl);
}

void bqLog::PrintError(const char* s, ...)
{
	Print("%s", "Error: ");
	va_list vl;
	va_start(vl, s);
	g_log.vprintf(s, vl);
	va_end(vl);
}

void bqLog::PrintError(const wchar_t* s, ...)
{
	Print("%s", "Error: ");
	va_list vl;
	va_start(vl, s);
	g_log.vwprintf(s, vl);
	va_end(vl);
}

// устанавливаем коллбэк, пользовательский или дефолтный
void bqLog::SetCallbackA(void(*cb)(const char*))
{
	if (cb)
		g_log.m_cba = cb;
	else
		g_log.m_cba = bqLogDefaultCallbackA;
}

void bqLog::SetCallbackW(void(*cb)(const wchar_t*))
{
	if (cb)
		g_log.m_cbw = cb;
	else
		g_log.m_cbw = bqLogDefaultCallbackW;
}