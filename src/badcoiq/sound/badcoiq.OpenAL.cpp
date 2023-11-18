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

#include "badcoiq.h"

#include "badcoiq/sound/bqSoundSystem.h"
#include "badcoiq.OpenAL.h"


extern "C"
{
	bqSoundEngine* BQ_CDECL bqSoundEngine_createOpenAL()
	{
		bqSoundEngineOpenAL* o = bqCreate<bqSoundEngineOpenAL>();
		return o;
	}
}

bqSoundEngineOpenAL::bqSoundEngineOpenAL()
{
	bqLog::PrintInfo("Init OpenAL...\n");
	m_device = alcOpenDevice(NULL);
	if (m_device)
	{
		m_context = alcCreateContext(m_device, NULL);
		alcMakeContextCurrent(m_context);
		m_eax = alIsExtensionPresent("EAX2.0");

		ALCint vmajor = 0;
		ALCint vminor = 0;
		alcGetIntegerv(m_device, ALC_MAJOR_VERSION, sizeof(vmajor), &vmajor);
		alcGetIntegerv(m_device, ALC_MINOR_VERSION, sizeof(vminor), &vminor);
		bqLog::PrintInfo("OpenAL version %i.%i\n", vmajor, vminor);
	
		if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE)
		{
			const ALCchar* devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
			if (devices)
			{
				bqLog::PrintInfo("Devices:\n");
				bqStringA stra;
				while (true)
				{
					stra.assign(devices);
					bqLog::PrintInfo("O:\t%s\n", stra.c_str());

					devices += stra.size();
					stra.clear();
					
					++devices;

					if (!*devices)
						break;
				}

				devices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
				if (devices)
				{
					while (true)
					{
						stra.assign(devices);
						bqLog::PrintInfo("I:\t%s\n", stra.c_str());

						devices += stra.size();
						stra.clear();

						++devices;

						if (!*devices)
							break;
					}
				}
			}

			const ALCchar* extensions = alcGetString(NULL, ALC_EXTENSIONS);
			if (extensions)
			{
				bqLog::PrintInfo("Extensions:\n");
				bqStringA stra;
				while (true)
				{
					stra.assign(extensions);
					bqLog::PrintInfo("\t%s\n", stra.c_str());

					extensions += stra.size();
					stra.clear();

					++extensions;

					if (!*extensions)
						break;
				}
			}
		}

		auto alstr = alcGetString(m_device, ALC_DEFAULT_DEVICE_SPECIFIER);
		if(alstr)
			bqLog::PrintInfo("Default Output: %s\n", alstr);

		alstr = alcGetString(m_device, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
		if (alstr)
			bqLog::PrintInfo("Default Input: %s\n", alstr);


		if(m_eax)
			bqLog::PrintInfo("EAX 2.0\n");
	}
	else
	{
		bqLog::PrintWarning("Failed: alcOpenDevice\n");
	}
}

bqSoundEngineOpenAL::~bqSoundEngineOpenAL()
{
	if (m_device)
	{
		alcMakeContextCurrent(NULL);
		alcDestroyContext(m_context);
		alcCloseDevice(m_device);
	}
}

const char* bqSoundEngineOpenAL::GetName()
{
	return "OpenAL";
}

bqSoundObject* bqSoundEngineOpenAL::SummonObject(bqSound* s)
{
	BQ_ASSERT_ST(s);
	BQ_PTR_D(bqSoundObjectOpenAL,o,new bqSoundObjectOpenAL());

	OpenALBuffer* _bufPtr = 0;

	for (size_t i = 0; i < m_buffers.m_size; ++i)
	{
		if (m_buffers.m_data[i].m_sound == s)
		{
			_bufPtr = &m_buffers.m_data[i];
			break;
		}
	}

	if (!_bufPtr)
	{
		OpenALBuffer _buf;
		_buf.m_sound = s;

		alGetError();
		alGenBuffers(1, &_buf.m_buffer);
		if (bqSoundEngineOpenAL::CheckOpenALError(alGetError()) != AL_NO_ERROR)
			return 0;

		ALenum format = 0;
		switch (s->m_soundSource->m_type)
		{
		case bqSoundSource::Type::uint8_mono_44100:
			format = AL_FORMAT_MONO8;
			break;
		case bqSoundSource::Type::uint8_stereo_44100:
			format = AL_FORMAT_STEREO8;
			break;
		case bqSoundSource::Type::uint16_mono_44100:
			format = AL_FORMAT_MONO16;
			break;
		case bqSoundSource::Type::uint16_stereo_44100:
			format = AL_FORMAT_STEREO16;
			break;
		}

		alBufferData(
			_buf.m_buffer,
			format,
			s->m_soundSource->m_sourceData.m_data,
			s->m_soundSource->m_sourceData.m_dataSize,
			s->m_soundSource->m_sourceInfo.m_sampleRate);

		if (bqSoundEngineOpenAL::CheckOpenALError(alGetError()) != AL_NO_ERROR)
		{
			alDeleteBuffers(1, &_buf.m_buffer);
			return 0;
		}
		m_buffers.push_back(_buf);
		_bufPtr = &m_buffers.m_data[m_buffers.m_size - 1];
	}


	if (o->Init(_bufPtr->m_buffer))
	{
		// Если всё ОК
		return o.Drop();
	}

	return 0;
}

ALenum bqSoundEngineOpenAL::CheckOpenALError(ALenum e)
{
	switch (e)
	{
	case AL_OUT_OF_MEMORY:
		bqLog::PrintError("OpenAL: AL_OUT_OF_MEMORY\n");
		break;
	case AL_INVALID_VALUE:
		bqLog::PrintError("OpenAL: AL_INVALID_VALUE\n");
		break;
	case AL_INVALID_ENUM:
		bqLog::PrintError("OpenAL: AL_INVALID_ENUM\n");
		break;
	case AL_INVALID_NAME:
		bqLog::PrintError("OpenAL: AL_INVALID_NAME\n");
		break;
	case AL_ILLEGAL_COMMAND:
		bqLog::PrintError("OpenAL: AL_ILLEGAL_COMMAND\n");
		break;
	case AL_NO_ERROR:
		break;
	}
	return e;
}

// ==========================================================
// 
// 
// 
// 
// ==========================================================

bqSoundObjectOpenAL::bqSoundObjectOpenAL()
{	
}

bqSoundObjectOpenAL::~bqSoundObjectOpenAL()
{
	if (m_source)
	{
		alSourceStop(m_source);
		alDeleteSources(1, &m_source);
	}
}

bool bqSoundObjectOpenAL::Init(ALuint buffer)
{
	alGenSources(1, &m_source);
	if (bqSoundEngineOpenAL::CheckOpenALError(alGetError()) != AL_NO_ERROR)
		return false;

	alSourcei(m_source, AL_BUFFER, buffer);

	return true;
}

void bqSoundObjectOpenAL::Play()
{
	alSourcePlay(m_source);
}

void bqSoundObjectOpenAL::Stop()
{
	alSourceStop(m_source);
}

void bqSoundObjectOpenAL::Pause()
{
	alSourcePause(m_source);
}

void bqSoundObjectOpenAL::Loop(bool loop)
{
	if (loop)
	{
		alSourcei(m_source, AL_LOOPING, AL_TRUE);
	}
	else 
	{
		alSourcei(m_source, AL_LOOPING, AL_FALSE);
	}
}
