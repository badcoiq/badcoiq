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
#ifndef __BQ_SFile_H__
#define __BQ_SFile_H__

#ifdef BQ_WITH_SOUND

// Этот класс будет открывать файлы со звуком
class bqSoundFile
{
	bqSoundBufferInfo m_info;
	FILE* m_file = 0;
	size_t m_dataSize = 0;
	long m_firstDataBlock = 0;
	long m_currentDataBlock = 0;

	using read_method = size_t(bqSoundFile::*)(void* buffer, size_t size);

	enum class file_type
	{
		wav
	}
	m_fileType = file_type::wav;
	
	read_method m_readMethod = 0;

	size_t _ReadWav(void* buffer, size_t size);
	size_t _ReadNull(void* buffer, size_t size);

	bool _OpenWAV(const char*);

public:
	bqSoundFile();
	~bqSoundFile();
	BQ_PLACEMENT_ALLOCATOR(bqSoundFile);

	bool Open(const char*);
	bool IsOpened() { return m_file != 0; }
	void Close();

	size_t Read(void* buffer, size_t size);

	const bqSoundBufferInfo& GetBufferInfo();

	enum class Type
	{
		Wav
	};

	Type GetType() { return m_type; }

	size_t GetDataSize() { return m_dataSize; }
	
	bqSoundFormat GetFormat();

	void MoveToFirstDataBlock();
	long Tell();
	void Seek(long);

	bool eof();

private:
	Type m_type = Type::Wav;
};


#endif
#endif


