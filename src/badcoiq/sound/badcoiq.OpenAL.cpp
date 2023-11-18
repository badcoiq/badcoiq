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
	return 0;
}

