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

#ifdef BQ_WITH_SOUND

#include "badcoiq/sound/bqSoundSystem.h"

#include "bqSoundStreamImpl.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqSoundStreamImpl::bqSoundStreamImpl()
{
}

bqSoundStreamImpl::~bqSoundStreamImpl()
{
}


void bqSoundStreamImpl::PlaybackStart()
{
}

void bqSoundStreamImpl::PlaybackStop()
{
}

void bqSoundStreamImpl::PlaybackReset()
{
}

void bqSoundStreamImpl::PlaybackSet(uint32_t minutes, float seconds)
{
}

void bqSoundStreamImpl::PlaybackSet(float secondsOnly)
{
}

bool bqSoundStreamImpl::Open(const char* path)
{
	BQ_ASSERT_ST(path);
	if (IsOpened())
		return false;

	if (!path)
		return false;

	fopen_s(&m_file, path, "rb");
	if(!m_file)
		return false;

	// надо проверить тип файла, поддерживает ли движок аудио данные и т.д.
	// скорее всего лучше проверять по содержимому, игнорировать расширение файла.

	char first4bytes[4] = { 0,0,0,0 };
	if (fread(first4bytes, 1, 4, m_file) != 4)
	{
		Close();
		return false;
	}

	enum class file_type
	{
		null,
		riff,
	}
	fileType = file_type::null;

	if (strcmp(first4bytes, "RIFF") == 0)
	{
		fileType = file_type::riff;
	}

	if (fileType == file_type::null)
	{
		Close();
		return false;
	}

	bool checkResult = false;
	switch (fileType)
	{
	case file_type::riff:
		checkResult = _check_riff();
		break;
	default:
		break;
	}

	if (!checkResult)
	{
		Close();
		return false;
	}

	switch (fileType)
	{
	case file_type::riff:
		
		// тут установка указателя на метод для чтения WAV

		break;
	default:
		break;
	}

	return true;
}

void bqSoundStreamImpl::Close()
{
	if (m_file)
	{
		fclose(m_file);
		m_file = 0;
	}
}

bool bqSoundStreamImpl::IsOpened()
{
	return m_file != 0;
}

bool bqSoundStreamImpl::_check_riff()
{
	// проверяем формат WAV, поддерживаем ли его
	return false;
}

#endif
