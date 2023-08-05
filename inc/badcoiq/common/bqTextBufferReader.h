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
#ifndef __BQ_TEXTBUFREAD_H__
#define __BQ_TEXTBUFREAD_H__

#include <string>

#include "badcoiq/string/bqString.h"

// Для удобного чтения ANSI текста из буфера
class bqTextBufferReader
{
	uint8_t* m_ptrCurr = 0;
	uint8_t* m_ptrBegin = 0;
	uint8_t* m_ptrEnd = 0;
	size_t m_size = 0;
	void _onConstructor()
	{
		m_ptrBegin = m_ptrCurr;
		m_ptrEnd = m_ptrCurr + m_size;
	}

	void _skipSpaces() {
		while (!IsEnd()) {
			if (!isspace(*m_ptrCurr))
				break;
			++m_ptrCurr;
		}
	}

	bool _isdelim(const char* delim, char ch)
	{
		if (delim)
		{
			const char* ptr = delim;
			while (*ptr)
			{
				if (*ptr == ch)
					return true;

				++ptr;
			}
		}
		return false;
	}

	bqStringA m_straTmp;

public:
	bqTextBufferReader() {}
	bqTextBufferReader(uint8_t* buffer, size_t size) :m_ptrCurr(buffer), m_size(size) { _onConstructor(); }
	bqTextBufferReader(char* buffer, size_t size) :m_ptrCurr((uint8_t*)buffer), m_size(size) { _onConstructor(); }
	bqTextBufferReader(char8_t* buffer, size_t size) :m_ptrCurr((uint8_t*)buffer), m_size(size) { _onConstructor(); }

	~bqTextBufferReader()
	{
	}

	void SkipSpaces() { _skipSpaces(); }

	void Set(uint8_t* buffer, size_t size)
	{
		m_ptrCurr = buffer;
		m_size = size;
		_onConstructor();
	}
	void Set(char* buffer, size_t size)
	{
		Set((uint8_t*)buffer, size);
	}
	void Set(char8_t* buffer, size_t size)
	{
		Set((uint8_t*)buffer, size);
	}

	bool IsEnd()
	{
		return (m_ptrCurr >= m_ptrEnd);
	}

	void SkipLine()
	{
		while (!IsEnd())
		{
			if (*m_ptrCurr == '\n')
			{
				++m_ptrCurr;
				break;
			}
			++m_ptrCurr;
		}
	}

	// Получить всё между символами isspace()
	void GetWord(bqStringA& out, const char* delim)
	{
		_skipSpaces();
		out.clear();
		while (!IsEnd())
		{
			if (isspace(*m_ptrCurr) || _isdelim(delim, *m_ptrCurr))
				break;
			out.push_back(*m_ptrCurr);
			++m_ptrCurr;
		}
	}

	// Подобно GetWord но позиция сохраняется
	void PickWord(bqStringA& out)
	{
		uint8_t* save = m_ptrCurr;
		GetWord(out, 0);
		m_ptrCurr = save;
	}

	char PickChar()
	{
		return *m_ptrCurr;
	}

	char GetChar()
	{
		char r = 0;
		if (*m_ptrCurr)
		{
			r = *m_ptrCurr;
			++m_ptrCurr;
		}
		return r;
	}

	int GetInt()
	{
		GetWord(m_straTmp, "/\\|");
		return atoi(m_straTmp.c_str());
	}

	float GetFloat()
	{
		GetWord(m_straTmp, "/\\|");
		return (float)atof(m_straTmp.c_str());
	}

	// Из этого: "bone01 abc"
	//  получить это: bone01 abc
	// Если первый символ не " тогда выход из метода
	void GetString(bqStringA& out)
	{
		out.clear();
		_skipSpaces();

		if (*m_ptrCurr == '\"')
		{
			++m_ptrCurr;
			while (!IsEnd())
			{
				if (*m_ptrCurr == '\"')
				{
					++m_ptrCurr;
					break;
				}
				out.push_back(*m_ptrCurr);
				++m_ptrCurr;
			}
		}
	}

	// Получить остаток текущей строки
	void GetLine(bqStringA& out)
	{
		out.clear();
		_skipSpaces();

		while (!IsEnd())
		{
			if (*m_ptrCurr == '\r' || *m_ptrCurr == '\n')
			{
				SkipLine();
				break;
			}
			out.push_back(*m_ptrCurr);
			++m_ptrCurr;
		}
	}

};

#endif

