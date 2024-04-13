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
#ifndef __BQ_FILEBUFFER_H__
#define __BQ_FILEBUFFER_H__

// Класс работает как FILE но с буфером.
// Если использовать просто FILE, можно использовать функции типа setbuf
// но ftell даст 0.
// Это класс для чтения
// Этот класс не выделяет память. Нужно передать указатель на буфер и размер буфера.
class bqFileBuffer
{
	uint8_t* m_buffer = 0;
	size_t m_size = 0;
	size_t m_cursor = 0;
public:
	bqFileBuffer(const uint8_t* buffer, size_t size)
		:
		m_buffer((uint8_t*)buffer),
		m_size(size)
	{
	}

	~bqFileBuffer()
	{
	}

	// Use SEEK_SET and other known macros for second parameter
	void Seek(size_t offset, int where)
	{
		switch (where)
		{
		default:
		case SEEK_CUR:
		{
			m_cursor += offset;
			if (m_cursor > m_size)
				m_cursor = m_size;
		}break;
		case SEEK_END:
			m_cursor = m_size;
			break;
		case SEEK_SET:
			m_cursor = offset;
			break;
		}
	}

	size_t Tell()
	{
		return m_cursor;
	}

	size_t Read(void* buffer, size_t size)
	{
		uint8_t* bytes = (uint8_t*)buffer;

		size_t numRead = 0;
		for (numRead = 0; numRead < size; ++numRead)
		{
			if (m_cursor == m_size)
				return numRead;

			bytes[numRead] = m_buffer[m_cursor];
			++m_cursor;
		}
		return numRead;
	}

	size_t Size()
	{
		return m_size;
	}

	bool Eof()
	{
		return m_cursor == m_size;
	}
};

#endif

