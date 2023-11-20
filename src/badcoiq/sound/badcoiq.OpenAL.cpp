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

#include "badcoiq/math/bqMath.h"


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
	//	m_eax = alIsExtensionPresent("EAX2.0");

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
		switch (s->m_soundSource->m_format)
		{
		case bqSoundFormat::uint8_mono_44100:
			format = AL_FORMAT_MONO8;
			break;
		case bqSoundFormat::uint8_stereo_44100:
			format = AL_FORMAT_STEREO8;
			break;
		case bqSoundFormat::uint16_mono_44100:
			format = AL_FORMAT_MONO16;
			break;
		case bqSoundFormat::uint16_stereo_44100:
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

bqSoundStreamObject* bqSoundEngineOpenAL::SummonStreamObject(const bqStringA& fn)
{
	return SummonStreamObject(fn.c_str());
}

bqSoundStreamObject* bqSoundEngineOpenAL::SummonStreamObject(const char* fn)
{
	BQ_ASSERT_ST(fn);

	BQ_PTR_D(bqSoundFile,new_sf,new bqSoundFile());

	if (new_sf->Open(fn))
	{
		bqSoundStreamObjectOpenAL* o = new bqSoundStreamObjectOpenAL(new_sf.Drop());
		// может быть тут нужны ещё какие проверки
		return o;
	}
	return 0;
}

void bqSoundEngineOpenAL::Test(const bqVec3& pos)
{
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	float directionvect[6];
	directionvect[0] = 1.f;
	directionvect[1] = 0;
	directionvect[2] = 0.f;
	directionvect[3] = 0;
	directionvect[4] = 1;
	directionvect[5] = 0;
	alListenerfv(AL_ORIENTATION, directionvect);

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
	
	float pos[3] = { 0.f,0.f,0.f };
	float vel[3] = { 1.f,0.f,0.f };
	alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
	alSourcefv(m_source, AL_POSITION, pos);
	alSourcefv(m_source, AL_VELOCITY, vel);
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



// =====================================================
// 
// 
// 
// =====================================================
void bqSoundStreamObjectOpenAL_thread(bqSoundStreamObjectOpenAL* context)
{
	while (context->m_threadRun)
	{
		switch (context->m_cmdTh)
		{
		case bqSoundStreamObjectOpenAL::CommandForThread::Play:
		{
			if (context->m_pause)
			{
				context->m_pause = false;
				context->m_soundFile->Seek(context->m_pausePos);
			}
			else
			{
				alSourceStop(context->m_source);
				ALint iBuffersProcessed = 0;
				ALuint b = 0;
				alGetSourcei(context->m_source, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
				for (int iLoop = 0; iLoop < iBuffersProcessed; ++iLoop)
					alSourceUnqueueBuffers(context->m_source, 1, &b);
				alSourcei(context->m_source, AL_BUFFER, 0);
			
				for (int iLoop = 0; iLoop < 4; ++iLoop)
				{
					uint8_t buffer[0xfffe];

					size_t readNum = context->m_soundFile->Read(buffer, 0xfffe);

					if (readNum)
					{
						alBufferData(context->m_buffers[iLoop], context->m_format, buffer, readNum, context->m_soundFile->GetSourceInfo().m_sampleRate);
						if (bqSoundEngineOpenAL::CheckOpenALError(alGetError()) != AL_NO_ERROR)
							return;

						alSourceQueueBuffers(context->m_source, 1, &context->m_buffers[iLoop]);
						if (bqSoundEngineOpenAL::CheckOpenALError(alGetError()) != AL_NO_ERROR)
							return;
					}
				}
			}

			alSourcePlay(context->m_source);
			if (bqSoundEngineOpenAL::CheckOpenALError(alGetError()) != AL_NO_ERROR)
				return;
			context->m_cmdTh = bqSoundStreamObjectOpenAL::CommandForThread::Null;
			context->m_state = bqSoundStreamObject::state_playing;
		}break;
		case bqSoundStreamObjectOpenAL::CommandForThread::Stop:
		{
			alSourceStop(context->m_source);
			context->m_cmdTh = bqSoundStreamObjectOpenAL::CommandForThread::Null;
			context->m_state = bqSoundStreamObject::state_notplaying;
		}break;
		case bqSoundStreamObjectOpenAL::CommandForThread::Pause:
		{
			//alSourceStop(context->m_source);
			context->m_cmdTh = bqSoundStreamObjectOpenAL::CommandForThread::Null;
			context->m_state = bqSoundStreamObject::state_notplaying;
			alSourcePause(context->m_source);
			context->m_pause = true;
			context->m_pausePos = context->m_soundFile->Tell();
		}break;
		}


		if (context->m_state == bqSoundStreamObject::state_playing
			&& !context->m_pause)
		{
			ALint iBuffersProcessed = 0;
			alGetSourcei(context->m_source, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
			while (iBuffersProcessed)
			{
				// Remove the Buffer from the Queue.  (uiBuffer contains the Buffer ID for the unqueued Buffer)
				ALuint uiBuffer = 0;
				alSourceUnqueueBuffers(context->m_source, 1, &uiBuffer);

				// Read more audio data (if there is any)
				uint8_t buffer[0xfffe];
				size_t readNum = context->m_soundFile->Read(buffer, 0xfffe);
				if (readNum)
				{
					alBufferData(
						uiBuffer,
						context->m_format,
						buffer,
						readNum,
						context->m_soundFile->GetSourceInfo().m_sampleRate);
					alSourceQueueBuffers(context->m_source, 1, &uiBuffer);
				}

				iBuffersProcessed--;
			}

			ALint iState = 0;
			alGetSourcei(context->m_source, AL_SOURCE_STATE, &iState);
			if (iState != AL_PLAYING)
			{
				ALint iQueuedBuffers = 0;
				alGetSourcei(context->m_source, AL_BUFFERS_QUEUED, &iQueuedBuffers);
				if (iQueuedBuffers)
				{
					alSourcePlay(context->m_source);
				}
				else
				{
					alSourceStop(context->m_source);

					if (context->m_loop)
					{
						context->m_cmdTh = bqSoundStreamObjectOpenAL::CommandForThread::Play;
						context->m_soundFile->MoveToFirstDataBlock();
					}
					else
					{
						context->m_cmdTh = bqSoundStreamObjectOpenAL::CommandForThread::Null;
						context->m_state = bqSoundStreamObject::state_notplaying;
					}
				}
			}
		}
	}
}

bqSoundStreamObjectOpenAL::bqSoundStreamObjectOpenAL(bqSoundFile* sf)
	:
	m_soundFile(sf)
{
	m_threadRun = true;
	m_thread = new std::thread(bqSoundStreamObjectOpenAL_thread, this);

	alGenBuffers(4, m_buffers);
	if (bqSoundEngineOpenAL::CheckOpenALError(alGetError()) != AL_NO_ERROR)
		return;

	alGenSources(1, &m_source);
	if (bqSoundEngineOpenAL::CheckOpenALError(alGetError()) != AL_NO_ERROR)
		return;

	auto bqformat = sf->GetFormat();

	switch (bqformat)
	{
	case bqSoundFormat::uint8_mono_44100:
		m_format = AL_FORMAT_MONO8;
		break;
	case bqSoundFormat::uint8_stereo_44100:
		m_format = AL_FORMAT_STEREO8;
		break;
	case bqSoundFormat::uint16_mono_44100:
		m_format = AL_FORMAT_MONO16;
		break;
	case bqSoundFormat::uint16_stereo_44100:
		m_format = AL_FORMAT_STEREO16;
		break;
	}

	
}

bqSoundStreamObjectOpenAL::~bqSoundStreamObjectOpenAL() 
{
	if (m_thread)
	{
		m_threadRun = false;
		m_thread->join();
		delete m_thread;
	}

	if (m_source)
		alDeleteSources(1, &m_source);

	if (m_buffers)
		alDeleteBuffers(4, m_buffers);

	if (m_soundFile)
		delete m_soundFile;
}

void bqSoundStreamObjectOpenAL::Play()
{
	if (m_state != bqSoundStreamObject::state_playing)
	{
		m_cmdTh = CommandForThread::Play;
		m_state = bqSoundStreamObject::state_notplaying;
	}
}

void bqSoundStreamObjectOpenAL::Stop()
{
	m_cmdTh = CommandForThread::Stop;
	m_soundFile->MoveToFirstDataBlock();
	m_pause = false;
}

void bqSoundStreamObjectOpenAL::Pause()
{
	if (m_state == bqSoundStreamObject::state_playing)
	{
		m_cmdTh = CommandForThread::Pause;
		
	}
}

void bqSoundStreamObjectOpenAL::Loop(bool v)
{
	m_loop = v;
}
