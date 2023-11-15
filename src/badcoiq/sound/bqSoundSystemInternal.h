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
#ifndef _BQ_SNDSSTMINT_H_
#define _BQ_SNDSSTMINT_H_

#include <thread>
#include <mutex>

#include "badcoiq/sound/bqSoundSystem.h"

namespace bq
{
	class SoundObjectCallback : public bqSoundObjectCallback
	{
	public:
		SoundObjectCallback();
		virtual ~SoundObjectCallback();

		virtual void OnStart() override;
		virtual void OnStop() override;
	};

	struct SoundInputThreadData
	{
		bqSoundObject* m_soundObject = 0;

		enum
		{
			command_null = 0,
			command_play,
			command_stop,
			command_pause,
			command_stopAll,
		};
		uint32_t m_command = 0;

		enum
		{
			playState_stop,
			playState_play,
			playState_pause,
			playState_unpause,
		//	playState_res,
			playState_remove,
		};
		uint32_t m_playState = playState_stop; // воспроизведение начинается с подготовки буфера

		enum
		{
			bufferState_prepare,
			bufferState_ready,
			bufferState_skip,
			//bufferState_last,
		};
		uint32_t m_bufferState = bufferState_prepare;

		uint32_t m_currentBuffer = 0;
		SoundObjectCallback m_internalCallback;

		// Копирование буфера начинается с места куда указывает m_sourceOffset.
		uint32_t m_sourceOffset = 0;
		// размер того буфера который идёт на воспроизведение
		uint32_t m_bufferSize = 0;
		uint32_t m_bufferSizeAdd = 0;

		// если воспроизводится последний буфер то должно быть true
		// или если достаточно только 1го буфера
		bool m_lastBuffer = false;


		void Effects();
	};
}

#endif
