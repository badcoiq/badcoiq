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
#ifndef __BQ_FILE_H__
#define __BQ_FILE_H__

class bqFile
{
	FILE* m_file = 0;
public:
	bqFile()
	{
	}

	~bqFile()
	{
		Close();
	}

	enum class _seek
	{
		cur,
		end,
		set,
	};

	void Seek(size_t offset, _seek where)
	{
		BQ_ASSERT_ST(m_file);
		int origin = SEEK_CUR;
		switch (where){
		case bqFile::_seek::end:
			origin = SEEK_END;
			break;
		case bqFile::_seek::set:
			origin = SEEK_SET;
			break;
		default:
			break;
		}
		fseek(m_file, offset, origin);
	}

	size_t Tell()
	{
		BQ_ASSERT_ST(m_file);
		return ftell(m_file);
	}

	size_t Read(void* buffer, size_t size)
	{
		BQ_ASSERT_ST(m_file);
		return fread_s(buffer, size, 1, size, m_file);
	}

	size_t Write(void* buffer, size_t size)
	{
		BQ_ASSERT_ST(m_file);
		return fwrite(buffer, 1, size, m_file);
	}

	size_t WriteUint8(uint8_t v)
	{
		BQ_ASSERT_ST(m_file);
		return fwrite(&v, 1, 1, m_file);
	}

	size_t WriteUint16(uint16_t v)
	{
		BQ_ASSERT_ST(m_file);
		return fwrite(&v, 1, 2, m_file);
	}

	size_t WriteUint32(uint32_t v)
	{
		BQ_ASSERT_ST(m_file);
		return fwrite(&v, 1, 4, m_file);
	}

	size_t WriteUint64(uint64_t v)
	{
		BQ_ASSERT_ST(m_file);
		return fwrite(&v, 1, 8, m_file);
	}

	size_t WriteInt8(int8_t v)
	{
		BQ_ASSERT_ST(m_file);
		return fwrite(&v, 1, 1, m_file);
	}

	size_t WriteInt16(int16_t v)
	{
		BQ_ASSERT_ST(m_file);
		return fwrite(&v, 1, 2, m_file);
	}

	size_t WriteInt32(int32_t v)
	{
		BQ_ASSERT_ST(m_file);
		return fwrite(&v, 1, 4, m_file);
	}

	size_t WriteInt64(int64_t v)
	{
		BQ_ASSERT_ST(m_file);
		return fwrite(&v, 1, 8, m_file);
	}

	size_t WriteFloat32(float32_t v)
	{
		BQ_ASSERT_ST(m_file);
		return fwrite(&v, 1, 4, m_file);
	}

	size_t WriteFloat64(float64_t v)
	{
		BQ_ASSERT_ST(m_file);
		return fwrite(&v, 1, 8, m_file);
	}

	size_t WriteString(const char* str)
	{
		BQ_ASSERT_ST(m_file);
		return WriteString(str, strlen(str));
	}

	size_t WriteString(const char* str, uint32_t size)
	{
		BQ_ASSERT_ST(m_file);
		return fwrite(str, 1, size, m_file);
	}

	size_t Size()
	{
		BQ_ASSERT_ST(m_file);
		size_t size = 0;
		auto position = Tell();
		Seek(0, _seek::end);
		size = Tell();
		Seek(position, _seek::set);
		return size;
	}

	bool Eof()
	{
		BQ_ASSERT_ST(m_file);
		return feof(m_file);
	}

	void Close()
	{
		BQ_ASSERT_ST(m_file);
		if (m_file)
		{
			fclose(m_file);
			m_file = 0;
		}
	}

	enum class _open
	{
		Open,
		OpenAppend,
		Create,
	};

	bool Open(_open op, const char* path)
	{
		BQ_ASSERT_ST(!m_file);
		BQ_ASSERT_ST(path);

		if (m_file)
			return false;

		const char* mode_rb = "rb";
		const char* mode_ab = "ab";
		const char* mode_wb = "wb";

		const char* mode = mode_rb;

		switch (op)
		{
		case bqFile::_open::Open:
			mode = mode_rb;
			break;
		case bqFile::_open::OpenAppend:
			mode = mode_ab;
			break;
		case bqFile::_open::Create:
			mode = mode_wb;
			break;
		default:
			break;
		}

		fopen_s(&m_file, path, mode);

		return true;
	}

	int GetC()
	{
		BQ_ASSERT_ST(m_file);
		return fgetc(m_file);
	}

	int UnGetC(int ch)
	{
		BQ_ASSERT_ST(m_file);
		return ungetc(ch, m_file);
	}
};

#endif

