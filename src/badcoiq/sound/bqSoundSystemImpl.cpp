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
#include "bqSoundStreamFileWAV.h"
#include "bqSoundStreamFileOGG.h"
#include "bqSoundSystemImpl.h"
#include "bqSoundMixerImpl.h"
#include "bqSoundEffectVolumeImpl.h"
#include "bqSoundEffectDelayImpl.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;


#include <strsafe.h>

bqSoundSystemImpl::bqSoundSystemImpl()
{
}

bqSoundSystemImpl::~bqSoundSystemImpl()
{
	bqLog::PrintInfo("Shutdown Sound System\n");
	if (m_WASAPIrenderer) delete m_WASAPIrenderer;
	if (m_device) m_device->Release();
}

bqSoundSystemDeviceInfo bqSoundSystemImpl::GetDeviceInfo()
{
	return m_deviceInfo;
}

bqSoundMixer* bqSoundSystemImpl::SummonMixer(uint32_t channels)
{
	BQ_ASSERT_ST(channels);
	bqSoundMixerImpl* newMixer = new bqSoundMixerImpl(channels, m_deviceInfo);
	return newMixer;
}

void bqSoundSystemImpl::AddMixerToProcessing(bqSoundMixer* mixer)
{
	BQ_ASSERT_ST(mixer);
	BQ_ASSERT_ST(m_WASAPIrenderer);
	m_WASAPIrenderer->ThreadCommand_AddMixer(reinterpret_cast<bqSoundMixerImpl*>(mixer));
}

void bqSoundSystemImpl::RemoveAllMixersFromProcessing()
{
	BQ_ASSERT_ST(m_WASAPIrenderer);

	m_WASAPIrenderer->ThreadCommand_RemoveAllMixers();
}

bqSoundEffectVolume* bqSoundSystemImpl::SummonEffectVolume()
{
	bqSoundEffectVolumeImpl* newEffect = new bqSoundEffectVolumeImpl();
	return newEffect;
}

bqSoundEffectDelay* bqSoundSystemImpl::SummonEffectDelay(uint32_t steps, uint32_t time)
{
	bqSoundEffectDelayImpl* newEffect = new bqSoundEffectDelayImpl(steps, time);
	return newEffect;
}

//  Retrieves the device friendly name for a particular device in a device collection.
LPWSTR GetDeviceName(IMMDeviceCollection* DeviceCollection, UINT DeviceIndex)
{
	IMMDevice* device;
	LPWSTR deviceId;
	HRESULT hr;

	hr = DeviceCollection->Item(DeviceIndex, &device);
	if (FAILED(hr))
	{
		printf("Unable to get device %d: %x\n", DeviceIndex, hr);
		return NULL;
	}
	hr = device->GetId(&deviceId);
	if (FAILED(hr))
	{
		printf("Unable to get device %d id: %x\n", DeviceIndex, hr);
		return NULL;
	}

	IPropertyStore* propertyStore;
	hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
	if (device)
	{
		device->Release();
		device = 0;
	}
	if (FAILED(hr))
	{
		printf("Unable to open device %d property store: %x\n", DeviceIndex, hr);
		return NULL;
	}

	PROPVARIANT friendlyName;
	PropVariantInit(&friendlyName);
	hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
	if (propertyStore)
	{
		propertyStore->Release();
		propertyStore = 0;
	}

	if (FAILED(hr))
	{
		printf("Unable to retrieve friendly name for device %d : %x\n", DeviceIndex, hr);
		return NULL;
	}

	wchar_t deviceName[128];
	hr = StringCbPrintf(deviceName, sizeof(deviceName), L"%s (%s)", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal, deviceId);
	if (FAILED(hr))
	{
		printf("Unable to format friendly name for device %d : %x\n", DeviceIndex, hr);
		return NULL;
	}

	PropVariantClear(&friendlyName);
	CoTaskMemFree(deviceId);

	wchar_t* returnValue = _wcsdup(deviceName);
	if (returnValue == NULL)
	{
		printf("Unable to allocate buffer for return\n");
		return NULL;
	}
	return returnValue;
}

bool bqSoundSystemImpl::Init()
{
	bool retValue = true;
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* deviceCollection = NULL;
	bqLog::PrintInfo("Init sound system...\n");

	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr))
	{
		bqLog::PrintError("Unable to instantiate device enumerator: %x\n", hr);
		retValue = false;
	}


	hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, 
		&deviceCollection);
	if (FAILED(hr))
	{
		bqLog::PrintError("Unable to retrieve device collection: %x\n", hr);
		retValue = false;
	}

	UINT deviceCount;
	hr = deviceCollection->GetCount(&deviceCount);
	if (FAILED(hr))
	{
		bqLog::PrintError("Unable to get device collection length: %x\n", hr);
		retValue = false;
	}

	bqLog::PrintInfo("Output devices:\n");
	for (UINT i = 0; i < deviceCount; i += 1)
	{
		LPWSTR deviceName;

		deviceName = GetDeviceName(deviceCollection, i);
		if (deviceName == NULL)
		{
			retValue = false;
		}
		else
		{
			bqLog::PrintInfo("    %d:  %S\n", i + 1, deviceName);
			free(deviceName);
		}
	}

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_device);
	if (FAILED(hr))
	{
		retValue = false;
		bqLog::PrintInfo("GetDefaultAudioEndpoint\n");
	}

	if (deviceCollection) deviceCollection->Release();
	if (deviceEnumerator) deviceEnumerator->Release();

	if (retValue)
	{
		m_WASAPIrenderer = new bqWASAPIRenderer();
		retValue = m_WASAPIrenderer->Initialize(m_device);
		if (!retValue)
		{
			delete m_WASAPIrenderer;
			m_WASAPIrenderer = 0;
		}
		else
		{
			m_deviceInfo.m_format = m_WASAPIrenderer->GetFormat();
			m_deviceInfo.m_channels = m_WASAPIrenderer->m_mixFormat->nChannels;
			m_deviceInfo.m_sampleRate = m_WASAPIrenderer->m_mixFormat->nSamplesPerSec;
			m_deviceInfo.m_bufferSize = m_WASAPIrenderer->m_bufferSize;
			m_deviceInfo.m_bitsPerSample = m_WASAPIrenderer->m_mixFormat->wBitsPerSample;

			bqLog::PrintInfo("Channels: %u\n", m_deviceInfo.m_channels);
			bqLog::PrintInfo("Samplerate: %u\n", m_deviceInfo.m_sampleRate);
			bqLog::PrintInfo("Bits: %u\n", m_deviceInfo.m_bitsPerSample);

			bqLog::PrintInfo("Format: ");
			switch (m_deviceInfo.m_format)
			{
			default:
			case bqSoundFormat::unsupported:
				bqLog::Print("unsupported");
				break;
			case bqSoundFormat::float32:
				bqLog::Print("float32");
				break;
			case bqSoundFormat::int16:
				bqLog::Print("int16");
				break;
			}
			bqLog::Print("\n");

		}

	}

	if (!retValue)
	{
		if (m_device)
		{
			m_device->Release();
			m_device = 0;
		}
	}
	return retValue;
}

bqSoundStream* bqSoundSystemImpl::SummonStream(const char* fn)
{
	BQ_ASSERT_ST(fn);
	if (!fn)
		return 0;

	bqSoundStreamFile* sf = 0;

	FILE* f = 0;
	fopen_s(&f, fn, "rb");

	if (f)
	{
		char name[5] = { 0,0,0,0,0 };
		fread(name, 1, 4, f);
		fclose(f);
		f = 0;

		if (strcmp(name, "RIFF") == 0)
		{
			bqSoundStreamFileWAV* sf_wav = new bqSoundStreamFileWAV;

			if (sf_wav->Open(fn))
				sf = sf_wav;
			else
				delete sf_wav;
		}
		else if (strcmp(name, "OggS") == 0)
		{
			bqSoundStreamFileOGG* sf_ogg = new bqSoundStreamFileOGG;
			if (sf_ogg->Open(fn))
				sf = sf_ogg;
			else
				delete sf_ogg;
		}
	}

	return sf ? new bqSoundStreamImpl(sf) : 0;
}

bqWASAPIRenderer::bqWASAPIRenderer()
{
	
}

bqWASAPIRenderer::~bqWASAPIRenderer()
{
	Shutdown();
}

void bqWASAPIRenderer::ThreadCommand_CallMethod(ThreadCommanMethodType method, _thread_command* cmd)
{
	(this->*method)(cmd);
}

void bqWASAPIRenderer::ThreadCommand_AddMixer(bqSoundMixerImpl* m)
{
	_thread_command cmd;
	cmd.m_ptr = m;
	cmd.m_method = &bqWASAPIRenderer::ThreadCommandMethod_AddMixer;
	this->m_threadContext.m_commands.Push(cmd);
}
void bqWASAPIRenderer::ThreadCommandMethod_AddMixer(_thread_command* cmd)
{
	m_threadContext.m_mixers.push_back((bqSoundMixerImpl*)cmd->m_ptr);
	m_threadContext.m_mainMixer->AddMixer((bqSoundMixerImpl*)cmd->m_ptr);
}
void bqWASAPIRenderer::ThreadCommand_SetMainMixer(bqSoundMixerImpl* m)
{
	_thread_command cmd;
	cmd.m_ptr = m;
	cmd.m_method = &bqWASAPIRenderer::ThreadCommandMethod_SetMainMixer;
	this->m_threadContext.m_commands.Push(cmd);
}
void bqWASAPIRenderer::ThreadCommandMethod_SetMainMixer(_thread_command* cmd)
{
	m_threadContext.m_mainMixer = (bqSoundMixerImpl*)cmd->m_ptr;
}
void bqWASAPIRenderer::ThreadCommand_RemoveAllMixers()
{
	m_threadContext.m_atomic_onRemoveAllMixers.store(1);

	_thread_command cmd;
	cmd.m_ptr = 0;
	cmd.m_method = &bqWASAPIRenderer::ThreadCommandMethod_RemoveAllMixers;
	this->m_threadContext.m_commands.Push(cmd);

	// тут надо подождать thread звука когда он удалит все миксеры
	while (true)
	{
		if(m_threadContext.m_atomic_onRemoveAllMixers.load() == 0)
			break;
	}
}
void bqWASAPIRenderer::ThreadCommandMethod_RemoveAllMixers(_thread_command* cmd)
{
	m_threadContext.m_mixers.clear();
	m_threadContext.m_mainMixer->m_mixers.clear();
	m_threadContext.m_atomic_onRemoveAllMixers.store(0);
}

void bqWASAPIRenderer::_thread_function()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		bqLog::PrintError("Sound thread: unable to initialize COM: %x\n", hr);
	}

	/*if (!DisableMMCSS)
	{
		mmcssHandle = AvSetMmThreadCharacteristics(L"Audio", &mmcssTaskIndex);
		if (mmcssHandle == NULL)
		{
			printf("Unable to enable MMCSS on render thread: %d\n", GetLastError());
		}
	}*/

	// очистка буфера у m_renderClient
		// чтобы при старте не проигрывался `мусор`
	{
		BYTE* pData;
		hr = m_renderClient->GetBuffer(m_bufferSize, &pData);
		if (FAILED(hr))
		{
			bqLog::PrintError("Failed to get buffer: %x.\n", hr);
			return;
		}
		hr = m_renderClient->ReleaseBuffer(m_bufferSize, AUDCLNT_BUFFERFLAGS_SILENT);
		if (FAILED(hr))
		{
			bqLog::PrintError("Failed to release buffer: %x.\n", hr);
			return;
		}

		UINT32 padding = 0;
		hr = m_audioClient->GetCurrentPadding(&padding);
		if (SUCCEEDED(hr))
		{
	//		bqLog::Print("first padding is %u\n", padding);
		}
	}

	hr = m_audioClient->Start();
	if (FAILED(hr))
	{
		bqLog::Print("Unable to start render client: %x.\n", hr);
	}
	else
	{
		//sound->m_threadState = bqSoundObjectImpl::ThreadState::ThreadState_play;
		//sound->m_state = bqSoundObject::state_playing;
		UINT32 padding = 0;
		hr = m_audioClient->GetCurrentPadding(&padding);
		if (SUCCEEDED(hr))
		{
			Sleep(50); // должна быть пауза между обновлениями буфера
			// видимо это то самое (хз что), о котором написано в примере
			// WASAPIRenderSharedEventDriven в CWASAPIRenderer::Initialize
			// Engine latency in shared mode timer driven cannot be less than 50ms
			// 

	//		bqLog::Print("padding on start is %u\n", padding);
			// так как обновили буфер из render client и запустили воспроизведение
			// padding равен какому-то значению, которое означает сколько байтов
			// осталось воспроизводить.
			// в буфер нужно послать звук. нужно создать отдельный метод для этого
			// и вызвать его в нескольких местах
			// первое место это здесь.
			// второе ниже, в bqSoundObjectImpl::ThreadState::ThreadState_play:
		//	sound->_thread_fillRenderBuffer();
		}
	}

	while (m_threadContext.m_run)
	{
		while (m_threadContext.m_commands.IsEmpty() == false)
		{
			auto command = m_threadContext.m_commands.Front();
			ThreadCommand_CallMethod(command.m_method, &command);
			m_threadContext.m_commands.Pop();
		}

		
		//Sleep(100);
		UINT32 padding = 0;
		hr = m_audioClient->GetCurrentPadding(&padding);
		if (SUCCEEDED(hr))
		{
			//bqLog::Print("padding %u\n", padding);
			//Sleep(1);
			//if(!padding)
			//sound->_thread_fillRenderBuffer();

			UINT32 framesAvailable = m_bufferSize - padding;
			UINT32 framesToWrite = m_bufferSize / m_frameSize;

			if (m_bufferSize <= (framesAvailable * m_frameSize))
			{
				for (size_t i = 0; i < m_threadContext.m_mixers.m_size; ++i)
				{
					auto currentMixer = m_threadContext.m_mixers.m_data[i];
					currentMixer->Process();
				}
				if (m_threadContext.m_mixers.m_size)
				{
					m_mainMixer->Process();

					BYTE* pData = 0;
					hr = m_renderClient->GetBuffer(framesToWrite, &pData);
					if (SUCCEEDED(hr))
					{

						uint32_t sampleSize = (m_mixFormat->nBlockAlign / m_mixFormat->nChannels);
						//uint32_t numOfBlocks = m_bufferSize / m_mixFormat->nBlockAlign;

						float* dst = (float*)pData;

						uint32_t numOfSamples = m_bufferSize / m_mixFormat->nBlockAlign;
						//uint32_t numOfSamplesPerChannel = numOfSamples / m_mixFormat->nChannels;

						uint32_t srcSampleIndex = 0;

						for (uint32_t si = 0; si < numOfSamples; ++si)
						{
							for (uint32_t ci = 0; ci < m_mixFormat->nChannels; ++ci)
							{

								// количество каналов в m_mainMixer или равно количеству каналов m_mixFormat
								// или 2 если количество каналов в m_mixFormat больше 2х
								uint32_t cci = ci;
								if (cci > 2)
									cci = 2;

								float* src = (float*)m_mainMixer->GetChannel(cci)->m_data;

								//	printf("Sample: %u Channel: %u Value: %f (%u)\n", si, ci, 
								//		src[srcSampleIndex], sampleSize);
									//*dst = src[srcSampleIndex];
									//++dst;

								uint8_t* dst8 = (uint8_t*)dst;
								uint8_t* src8 = (uint8_t*)&src[srcSampleIndex];

								for (uint32_t ssi = 0; ssi < sampleSize; ++ssi)
								{
									*dst8 = *src8;
									++dst8;
									++src8;
								}
								++dst;
							}

							++srcSampleIndex;
						}

						hr = m_renderClient->ReleaseBuffer(framesToWrite, 0);
						//printf("done %u\n", sound->m_currentPosition);
						if (!SUCCEEDED(hr))
						{
							//AUDCLNT_E_BUFFER_ERROR
							printf("Unable to release buffer: %x\n", hr);
							//	stillPlaying = false;
						}
					}
					else
					{
						printf("Unable to get buffer: %x bufferSize: %u\n", hr, m_bufferSize);
						//	stillPlaying = false;
					}
				}
			}
			else
			{
			}
		}
	}

	hr = m_audioClient->Stop();

	/*if (!DisableMMCSS)
	{
		AvRevertMmThreadCharacteristics(mmcssHandle);
	}*/

	CoUninitialize();
}

bool bqWASAPIRenderer::Initialize(IMMDevice* Endpoint)
{
	BQ_ASSERT_ST(!m_endpoint);
	BQ_ASSERT_ST(Endpoint);

	if (!m_endpoint)
	{
		m_endpoint = Endpoint;
		m_endpoint->AddRef();

		HRESULT hr = m_endpoint->Activate(__uuidof(IAudioClient),
			CLSCTX_INPROC_SERVER, NULL,
			reinterpret_cast<void**>(&m_audioClient));
		if (FAILED(hr))
		{
			bqLog::PrintError("Unable to activate audio client: %x.\n", hr);
			return false;
		}

		hr = m_audioClient->GetMixFormat(&m_mixFormat);
		if (FAILED(hr))
		{
			bqLog::PrintError("Unable to get mix format on audio client: %x.\n", hr);
			return false;
		}
		m_frameSize = m_mixFormat->nBlockAlign;

		//  If the mix format is a float format, just try to convert the format to PCM.
		if (m_mixFormat->wFormatTag == WAVE_FORMAT_PCM ||
			m_mixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
			reinterpret_cast<WAVEFORMATEXTENSIBLE*>(m_mixFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
		{
			if (m_mixFormat->wBitsPerSample == 16)
			{
				m_renderSampleType = SampleType16BitPCM;
			}
			else
			{
				bqLog::PrintError("Unknown PCM integer sample type\n");
				return false;
			}
		}
		else if (m_mixFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
			(m_mixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
				reinterpret_cast<WAVEFORMATEXTENSIBLE*>(m_mixFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
		{
			m_renderSampleType = SampleTypeFloat;
		}
		else
		{
			bqLog::PrintError("unrecognized device format.\n");
			return false;
		}

		uint32_t EngineLatency = 100;

		REFERENCE_TIME bufferDuration = EngineLatency * 10000;
		REFERENCE_TIME periodicity = EngineLatency * 10000;

		hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
			AUDCLNT_STREAMFLAGS_NOPERSIST,
			bufferDuration,
			periodicity,
			m_mixFormat,
			NULL);
		if (FAILED(hr))
		{
			bqLog::PrintError("Unable to initialize audio client: %x.\n", hr);
			return false;
		}

		//constexpr uint32_t ewerr = AUDCLNT_E_BUFFER_TOO_LARGE;
		// AUDCLNT_E_BUFFER_TOO_LARGE = decimal code 2290679814

		//  Retrieve the buffer size for the audio client.
		hr = m_audioClient->GetBufferSize(&m_bufferSize);
		if (FAILED(hr))
		{
			bqLog::PrintError("Unable to get audio client buffer: %x. \n", hr);
			return false;
		}

		hr = m_audioClient->GetService(IID_PPV_ARGS(&m_renderClient));
		if (FAILED(hr))
		{
			bqLog::PrintError("Unable to get new render client: %x.\n", hr);
			return false;
		}


		int ch = m_mixFormat->nChannels;
		if (ch > 2)
			ch = 2;
		bqSoundSystemDeviceInfo ssdi;
		ssdi.m_bufferSize = m_bufferSize;
		ssdi.m_channels = ch;
		ssdi.m_format = GetFormat();
		ssdi.m_sampleRate = m_mixFormat->nSamplesPerSec;
		m_mainMixer = new bqSoundMixerImpl(ch, ssdi);

		this->ThreadCommand_SetMainMixer(m_mainMixer);

		m_threadContext.m_run = true;
		m_thread = new std::thread(&bqWASAPIRenderer::_thread_function, this);

		return true;
	}
	return false;
}

bqSoundFormat bqWASAPIRenderer::GetFormat()
{
	bqSoundFormat f = bqSoundFormat::unsupported;

	switch (m_renderSampleType)
	{
	case bqWASAPIRenderer::SampleType16BitPCM:
		f = bqSoundFormat::int16;
		break;
	case bqWASAPIRenderer::SampleTypeFloat:
		f = bqSoundFormat::float32;
		break;
	}

	return f;
}

void bqWASAPIRenderer::Shutdown()
{
	bqLog::PrintInfo("Shutdown WASAPI\n");

	if (m_thread)
	{
		bqLog::PrintInfo("Shutdown audio thread...\n");
		m_threadContext.m_run = false;

		if(m_thread->joinable())
			m_thread->join();

		m_threadContext.m_commands.Clear();
		m_threadContext.m_mixers.clear();

		delete m_thread;
		m_thread = 0;

		if (m_audioClient)
		{
			m_audioClient->Release();
			m_audioClient = 0;
		}

		if (m_renderClient)
		{
			m_renderClient->Release();
			m_renderClient = 0;
		}
	}

	if (m_endpoint)
	{
		m_endpoint->Release();
		m_endpoint = 0;
	}

	if (m_mainMixer)
	{
		for (uint32_t i = 0; i < m_mainMixer->m_mixers.m_size; ++i)
		{
			delete m_mainMixer->m_mixers.m_data[i];
		}
		m_mainMixer->RemoveAllMixers();

		delete m_mainMixer;
		m_mainMixer = 0;
	}
}

//  When a session is disconnected because of a device removal or format change event, we just want 
//  to let the render thread know that the session's gone away
//
HRESULT bqWASAPIRenderer::OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason)
{
	if (DisconnectReason == DisconnectReasonDeviceRemoval)
	{
		//
		//  The stream was disconnected because the device we're rendering to was removed.
		//
		//  We want to reset the stream switch complete event (so we'll block when the HandleStreamSwitchEvent function
		//  waits until the default device changed event occurs).
		//
		//  Note that we _don't_ set the _StreamSwitchCompleteEvent - that will be set when the OnDefaultDeviceChanged event occurs.
		//
		//_InStreamSwitch = true;
		//SetEvent(_StreamSwitchEvent);
	}
	if (DisconnectReason == DisconnectReasonFormatChanged)
	{
		//
		//  The stream was disconnected because the format changed on our render device.
		//
		//  We want to flag that we're in a stream switch and then set the stream switch event (which breaks out of the renderer).  We also
		//  want to set the _StreamSwitchCompleteEvent because we're not going to see a default device changed event after this.
		//
		//_InStreamSwitch = true;
		//SetEvent(_StreamSwitchEvent);
		//SetEvent(_StreamSwitchCompleteEvent);
	}
	return S_OK;
}

//  Called when the default render device changed.  We just want to set an event which lets the stream switch logic know that it's ok to 
//  continue with the stream switch.
//
HRESULT bqWASAPIRenderer::OnDefaultDeviceChanged(EDataFlow /*Flow*/, ERole /*Role*/, LPCWSTR /*NewDefaultDeviceId*/)
{
	//if (Flow == eRender && Role == _EndpointRole)
	//{
	//    //
	//    //  The default render device for our configuredf role was changed.  
	//    //
	//    //  If we're not in a stream switch already, we want to initiate a stream switch event.  
	//    //  We also we want to set the stream switch complete event.  That will signal the render thread that it's ok to re-initialize the
	//    //  audio renderer.
	//    //
	//    if (!_InStreamSwitch)
	//    {
	//        _InStreamSwitch = true;
	//        SetEvent(_StreamSwitchEvent);
	//    }
	//    SetEvent(_StreamSwitchCompleteEvent);
	//}
	return S_OK;
}

//  IUnknown
HRESULT bqWASAPIRenderer::QueryInterface(REFIID Iid, void** Object)
{
	if (Object == NULL)
		return E_POINTER;
	*Object = NULL;

	if (Iid == IID_IUnknown)
	{
		*Object = static_cast<IUnknown*>(static_cast<IAudioSessionEvents*>(this));
		AddRef();
	}
	else if (Iid == __uuidof(IMMNotificationClient))
	{
		*Object = static_cast<IMMNotificationClient*>(this);
		AddRef();
	}
	else if (Iid == __uuidof(IAudioSessionEvents))
	{
		*Object = static_cast<IAudioSessionEvents*>(this);
		AddRef();
	}
	else
		return E_NOINTERFACE;
	return S_OK;
}

ULONG bqWASAPIRenderer::AddRef()
{
	return InterlockedIncrement(&_RefCount);
}

ULONG bqWASAPIRenderer::Release()
{
	ULONG returnValue = InterlockedDecrement(&_RefCount);
	if (returnValue == 0)
		delete this;
	return returnValue;
}

#endif
