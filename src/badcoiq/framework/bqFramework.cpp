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
#include "badcoiq/system/bqWindow.h"
#include "badcoiq/system/bqWindowWin32.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/common/bqImageLoader.h"

#include "badcoiq/common/bqTextBufferReader.h"

#include "bqFrameworkImpl.h"

#include <filesystem>

#include <algorithm>

//
//  Lowercases string
//
template <typename T>
std::basic_string<T> lowercase(const std::basic_string<T>& s)
{
	std::basic_string<T> s2 = s;
	std::transform(s2.begin(), s2.end(), s2.begin(), tolower);
	return s2;
}

//
// Uppercases string
//
template <typename T>
std::basic_string<T> uppercase(const std::basic_string<T>& s)
{
	std::basic_string<T> s2 = s;
	std::transform(s2.begin(), s2.end(), s2.begin(), toupper);
	return s2;
}

extern "C"
{
	bqGS* BQ_CDECL bqGSD3D11_create();
	bqImageLoader* BQ_CDECL bqImageLoaderDefault_create();
}
BQ_LINK_LIBRARY("badcoiq.d3d11");
BQ_LINK_LIBRARY("badcoiq.imageloader");

void bqInputUpdatePre();
void bqInputUpdatePost();

bqFrameworkImpl* g_framework = 0;

class bqFrameworkDestroyer
{
public:
	bqFrameworkDestroyer() {}
	~bqFrameworkDestroyer() 
	{
		if (g_framework)
			bqFramework::Stop();
	}
};
// При закрытии программы этот объект будет уничтожен - будет вызван деструктор
//   в котором будет вызван bqFramework::Stop();
bqFrameworkDestroyer g_frameworkDestroyer;

// Выделяю память
void bqFramework::Start(bqFrameworkCallback* cb)
{
	BQ_ASSERT_STC(cb, "You need to create callback class for bqFramework");
	BQ_ASSERT_STC(!g_framework, "You can call bqFramework::Start only once");
	if (!g_framework)
	{
		g_framework = new bqFrameworkImpl();

#ifdef BQ_PLATFORM_WINDOWS
		RAWINPUTDEVICE device;
		device.usUsagePage = 0x01;
		device.usUsage = 0x02;
		device.dwFlags = 0;
		device.hwndTarget = 0;
		RegisterRawInputDevices(&device, 1, sizeof device);
#endif

		g_framework->m_gss.push_back(bqGSD3D11_create());
		//g_framework->m_gss.push_back(bqGSD3D12_create());
		//g_framework->m_gss.push_back(bqGSVulkan_create());

		g_framework->m_imageLoaders.push_back(bqImageLoaderDefault_create());
	}
}

// Освобождаю
void bqFramework::Stop()
{
	BQ_ASSERT_STC(g_framework, "You can call bqFramework::Stop only after calling bqFramework::Start. Or the problem is in something else.");
	if (g_framework)
	{
		g_framework->OnDestroy();
		delete g_framework;
		g_framework = 0;
	}
}

void bqFrameworkImpl::OnDestroy()
{
	if (g_framework->m_imageLoaders.size())
	{
		for (auto o : g_framework->m_imageLoaders)
		{
			bqDestroy(o);
		}
		g_framework->m_imageLoaders.clear();
	}

	if (g_framework->m_gss.size())
	{
		for (auto o : g_framework->m_gss)
		{
			o->Shutdown();
			bqDestroy(o);
		}
		g_framework->m_gss.clear();
	}
}

void bqFramework::Update()
{
	BQ_ASSERT_STC(g_framework, "This method must be called only after framework initialization (bqFramework::Start)");

	bqInputUpdatePre();

#ifdef BQ_PLATFORM_WINDOWS
	// без этого окно не будет реагировать
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#endif

	bqInputUpdatePost();

	static clock_t then = 0;
	clock_t now = clock();

	g_framework->m_deltaTime = (float)(now - then) / CLOCKS_PER_SEC;
	then = now;
}

float* bqFramework::GetDeltaTime()
{
	BQ_ASSERT_STC(g_framework, "This method must be called only after framework initialization (bqFramework::Start)");
	return &g_framework->m_deltaTime;
}

bqWindow* bqFramework::SummonWindow(bqWindowCallback* cb)
{
	BQ_ASSERT_STC(g_framework, "This method must be called only after framework initialization (bqFramework::Start)");
	BQ_ASSERT_STC(cb, "You need to create callback class for window");
	return new bqWindow(cb);
}

// =========== GS
uint32_t bqFramework::GetGSNum()
{
	return (uint32_t)g_framework->m_gss.size();
}

bqString bqFramework::GetGSName(uint32_t i)
{
	BQ_ASSERT_STC(i < g_framework->m_gss.size(), "Bad index");
	return g_framework->m_gss[i]->GetName();
}

bqUID bqFramework::GetGSUID(uint32_t i)
{
	BQ_ASSERT_STC(i < g_framework->m_gss.size(), "Bad index");
	return g_framework->m_gss[i]->GetUID();
}

bqGS* bqFramework::SummonGS(bqUID id)
{
	for (auto o : g_framework->m_gss)
	{
		if (CompareUIDs(o->GetUID(), id))
			return o;
	}
	return 0;
}

bqGS* bqFramework::SummonGS(const char* _name)
{
	bqString name(_name);
	for (auto o : g_framework->m_gss)
	{
		bqString o_name = o->GetName();
		if (name == o_name)
			return o;
	}
	return 0;
}

bqGS* bqFramework::SummonGS(bqUID id, const char* _name)
{
	bqString name(_name);
	for (auto o : g_framework->m_gss)
	{
		if (CompareUIDs(o->GetUID(), id))
		{
			bqString o_name = o->GetName();
			if (name == o_name)
				return o;
		}
	}
	return 0;
}

bool bqFramework::CompareUIDs(const bqUID& id1, const bqUID& id2)
{
	const uint8_t* b1 = (const uint8_t*)&id1.d1;
	const uint8_t* b2 = (const uint8_t*)&id2.d1;
	for (int i = 0; i < 16; ++i)
	{
		if (b1[i] != b2[i])
			return false;
	}
	return true;
}

bqMat4* bqFramework::GetMatrix(bqMatrixType t)
{
	return g_framework->m_matrixPtrs[(uint32_t)t];
}

void bqFramework::SetMatrix(bqMatrixType t, bqMat4* m)
{
	g_framework->m_matrixPtrs[(uint32_t)t] = m;
}

uint8_t* bqFramework::SummonFileBuffer(const char* path, uint32_t* szOut, bool isText)
{
	BQ_ASSERT_ST(path);
	BQ_ASSERT_ST(szOut);

	*szOut = 0;

	std::filesystem::path p = path;

	if (std::filesystem::exists(p))
	{
		*szOut = (uint32_t)std::filesystem::file_size(p);
		if (*szOut)
		{
			FILE* f = 0;
			fopen_s(&f, path, "rb");
			if (f)
			{
				if (isText)
					*szOut += 2;

				uint8_t* data = (uint8_t*)bqMemory::malloc(*szOut);
				fread(data, *szOut, 1, f);
				fclose(f);

				if (isText)
				{
					data[*szOut - 2] = ' ';
					data[*szOut - 1] = 0;
				}

				return data;
			}
			else
			{
				bqLog::PrintError("Unable to open file in %s\n", BQ_FUNCTION);
			}
		}
	}
	return 0;
}

uint32_t bqFramework::GetImageLoadersNum()
{
	return (uint32_t)g_framework->m_imageLoaders.size();
}

bqImageLoader* bqFramework::GetImageLoader(uint32_t i)
{
	BQ_ASSERT_ST(i < g_framework->m_imageLoaders.size());
	return g_framework->m_imageLoaders[i];
}

bqImage* bqFramework::SummonImage(const char* path)
{
	bqStringA stra;
	std::filesystem::path p = path;
	auto e = p.extension();
	uint32_t mln = GetImageLoadersNum();
	for (uint32_t i = 0; i < mln; ++i)
	{
		auto il = GetImageLoader(i);
		auto sfc = il->GetSupportedFilesCount();
		for (uint32_t o = 0; o < sfc; ++o)
		{
			bqString sfe = il->GetSupportedFileExtension(o);
			sfe.insert(U".", 0);
			sfe.to_lower();
			sfe.to_utf8(stra);
			auto stre = lowercase(e.generic_string());
			if (strcmp((const char*)stra.data(), stre.c_str()) == 0)
			{
				bqLog::PrintInfo("Load image: %s\n", path);
				return il->Load(path);
			}
		}
	}
	return NULL;
}


