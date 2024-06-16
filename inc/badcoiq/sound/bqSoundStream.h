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
#ifndef __BQ_SOUNStream_H__
#define __BQ_SOUNStream_H__

#ifdef BQ_WITH_SOUND

class bqSoundStreamCallback
{
public:
	bqSoundStreamCallback() {}
	virtual ~bqSoundStreamCallback() {}


	virtual void OnEndStream() = 0;
};

class bqSoundStream
{
public:
	bqSoundStream() {}
	virtual ~bqSoundStream() {}
	BQ_PLACEMENT_ALLOCATOR(bqSoundStream);

	bool IsPlaying() { return m_state == state_playing; }

	// размер аудио данных целиком
	// в распакованном виде.
	// нужно вычислять при открытии файла
	virtual uint64_t GetDataSize() = 0;

	virtual void PlaybackStart() = 0;
	virtual void PlaybackStop() = 0;
	virtual void PlaybackReset()= 0;
	virtual void PlaybackSet(uint32_t minutes, float seconds)= 0;
	virtual void PlaybackSet(float secondsOnly)= 0;
	
	// открыть/закрыть файл
	virtual bool Open(const char*) = 0;
	virtual void Close() = 0;
	virtual bool IsOpened() = 0;
	
	// вернёт 2
	virtual uint32_t GetNumOfChannels() = 0;
	//	virtual const bqSoundBufferInfo& GetBufferInfo() = 0;
	virtual uint32_t GetBlockSize() = 0;
	
	virtual void SetCallback(bqSoundStreamCallback*) = 0;

	enum
	{
		state_notPlaying,
		state_playing,
	};
	uint32_t m_state = state_notPlaying;

	bool m_loop = false;
	float m_volume = 0.5f;
	bool m_use3D = false;
};


#endif
#endif

