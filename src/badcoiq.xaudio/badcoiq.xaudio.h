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
#ifndef __BQ_XAUDIO_H__
#define __BQ_XAUDIO_H__

#include "badcoiq.h"
#include "badcoiq/containers/bqArray.h"
#include "badcoiq/sound/bqSoundSystem.h"

#include <xaudio2.h>
#include <xaudio2fx.h>

#pragma warning(push)
#pragma warning(disable : 4619 4616 5246)
#include <x3daudio.h>
#pragma warning(pop)

#include <xapofx.h>

#undef PlaySound

class bqSoundObjectXAudio;
class bqIXAudio2VoiceCallback : public IXAudio2VoiceCallback
{
public:
	bqIXAudio2VoiceCallback() {}
	virtual ~bqIXAudio2VoiceCallback() {}

	STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired);
	STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS);
	STDMETHOD_(void, OnStreamEnd) (THIS);
	STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext);
	STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext);
	STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext);
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error);

	bqSoundObjectXAudio* m_so = 0;
};

class bqSoundObjectXAudio : public bqSoundObject
{
public:
	bqSoundObjectXAudio();
	virtual ~bqSoundObjectXAudio();
	BQ_PLACEMENT_ALLOCATOR(bqSoundObjectXAudio);

	IXAudio2SourceVoice* m_SourceVoice = 0;

	bqIXAudio2VoiceCallback* m_xaudioCallback = 0;

	UINT32 m_loopCount = 0;

	virtual void Start() override;
	virtual void Stop() override;
	virtual void Pause() override;
	virtual void SetVolume(float) override;
	virtual void EnableLoop(uint32_t loops) override;
	virtual void DisableLoop() override;

	virtual void SetSource(void* data, uint32_t dataSize) override;
	virtual void PlaySource() override;
	virtual void StopSource() override;

	virtual void Use3D() override;
};

class bqSoundEngineXAudio : public bqSoundEngine
{
	const XAUDIO2FX_REVERB_I3DL2_PARAMETERS m_ReverbPresets[31] =
	{
		XAUDIO2FX_I3DL2_PRESET_DEFAULT,             // Reverb_Off
		XAUDIO2FX_I3DL2_PRESET_DEFAULT,             // Reverb_Default
		XAUDIO2FX_I3DL2_PRESET_GENERIC,             // Reverb_Generic
		XAUDIO2FX_I3DL2_PRESET_FOREST,              // Reverb_Forest
		XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,          // Reverb_PaddedCell
		XAUDIO2FX_I3DL2_PRESET_ROOM,                // Reverb_Room
		XAUDIO2FX_I3DL2_PRESET_BATHROOM,            // Reverb_Bathroom
		XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,          // Reverb_LivingRoom
		XAUDIO2FX_I3DL2_PRESET_STONEROOM,           // Reverb_StoneRoom
		XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,          // Reverb_Auditorium
		XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,         // Reverb_ConcertHall
		XAUDIO2FX_I3DL2_PRESET_CAVE,                // Reverb_Cave
		XAUDIO2FX_I3DL2_PRESET_ARENA,               // Reverb_Arena
		XAUDIO2FX_I3DL2_PRESET_HANGAR,              // Reverb_Hangar
		XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY,     // Reverb_CarpetedHallway
		XAUDIO2FX_I3DL2_PRESET_HALLWAY,             // Reverb_Hallway
		XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,       // Reverb_StoneCorridor
		XAUDIO2FX_I3DL2_PRESET_ALLEY,               // Reverb_Alley
		XAUDIO2FX_I3DL2_PRESET_CITY,                // Reverb_City
		XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,           // Reverb_Mountains
		XAUDIO2FX_I3DL2_PRESET_QUARRY,              // Reverb_Quarry
		XAUDIO2FX_I3DL2_PRESET_PLAIN,               // Reverb_Plain
		XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,          // Reverb_ParkingLot
		XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,           // Reverb_SewerPipe
		XAUDIO2FX_I3DL2_PRESET_UNDERWATER,          // Reverb_Underwater
		XAUDIO2FX_I3DL2_PRESET_SMALLROOM,           // Reverb_SmallRoom
		XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,          // Reverb_MediumRoom
		XAUDIO2FX_I3DL2_PRESET_LARGEROOM,           // Reverb_LargeRoom
		XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,          // Reverb_MediumHall
		XAUDIO2FX_I3DL2_PRESET_LARGEHALL,           // Reverb_LargeHall
		XAUDIO2FX_I3DL2_PRESET_PLATE,               // Reverb_Plate
	};

	bool m_isInit = false;
	IXAudio2* m_XAudio = 0;
	IXAudio2MasteringVoice* m_MasteringVoice = 0;
	IXAudio2SubmixVoice* m_ReverbVoice = 0;
	X3DAUDIO_HANDLE m_X3DAudio;
	IUnknown* m_ReverbEffect = 0;
public:
	bqSoundEngineXAudio();
	virtual ~bqSoundEngineXAudio();

	virtual bqSoundObject* SummonSoundObject(bqSound*) override;
	virtual const char* Name() override;

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual bool IsReady() override;
};

#endif
