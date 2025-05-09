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

#ifdef BQ_WITH_WINDOW
#include "badcoiq/system/bqWindow.h"
#ifdef BQ_PLATFORM_WINDOWS
#include "badcoiq/system/bqWindowWin32.h"
#endif
#endif

#ifdef BQ_WITH_POPUP
#include "badcoiq/system/bqPopup.h"
#include "badcoiq/system/bqPopupWin32.h"
#endif

#ifdef BQ_WITH_GS
#include "badcoiq/gs/bqGS.h"
#endif

#ifdef BQ_WITH_GUI
#include "badcoiq/GUI/bqGUI.h"
#include "../GUI/bqGUIDefaultTextDrawCallbacks.h"

#ifdef BQ_PLATFORM_WINDOWS
#include "../GUI/bqGUIIconTexture_GDI.h"
#endif
#endif

#ifdef BQ_WITH_IMAGE
#include "badcoiq/common/bqImageLoader.h"
#endif

#include "badcoiq/geometry/bqPolygonMesh.h"
#include "badcoiq/geometry/bqMeshLoader.h"

#ifdef BQ_WITH_SOUND
#include "badcoiq/sound/bqSoundSystem.h"
#include "../sound/bqSoundSystemImpl.h"
class bqSoundObjectImpl;
#endif

#include "badcoiq/common/bqTextBufferReader.h"

#ifdef BQ_WITH_ARCHIVE
#include "badcoiq/archive/bqArchive.h"
#endif

#include "../system/bqCursorWin32.h"

#include "bqFrameworkImpl.h"

#include <filesystem>

#include <algorithm>

#ifdef BQ_WITH_IMAGE_PNG
static uint8_t g_defaultFontPNG[] = {
	#include "../_data/font.inl"
};
static uint8_t g_defaultIconsPNG[] = {
	#include "../_data/defaultIcons.inl"
};
#endif

#ifdef BQ_WITH_PHYSICS
#include "badcoiq/physics/bqPhysicsSystem.h"
#endif

#include "badcoiq/VG/bqVectorGraphics.h"
#include "badcoiq/VG/bqVectorGraphicsTarget.h"

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
#ifdef BQ_WITH_GS
	bqGS* BQ_CDECL bqGSD3D11_create();
#endif

#ifdef BQ_WITH_IMAGE
	bqImageLoader* BQ_CDECL bqImageLoaderDefault_create();
#endif

	bqMeshLoader* BQ_CDECL bqMeshLoaderDefault_create();
}


#ifdef BQ_WITH_GS
BQ_LINK_LIBRARY("badcoiq.d3d11");
#endif
#ifdef BQ_WITH_IMAGE
BQ_LINK_LIBRARY("badcoiq.imageloader");
#endif
BQ_LINK_LIBRARY("badcoiq.meshloader");

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
	bqLog::PrintInfo("Init BADCOIQ\n");
	if (!g_framework)
	{
		g_framework = new bqFrameworkImpl();

#ifdef BQ_WITH_GUI
		g_framework->_initGUIThemes();
		g_framework->_initGUITextDrawCallbacks();
#endif

#ifdef BQ_PLATFORM_WINDOWS
		wchar_t pth[1000];
		GetModuleFileName(0, pth, 1000);
		g_framework->m_appPath = pth;
		g_framework->m_appPath.pop_back_before(U'\\');
		g_framework->m_appPath.replace(U'\\', U'/');

		RAWINPUTDEVICE device;
		device.usUsagePage = 0x01;
		device.usUsage = 0x02;
		device.dwFlags = 0;
		device.hwndTarget = 0;
		RegisterRawInputDevices(&device, 1, sizeof device);

		HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (FAILED(hr))
		{
			bqLog::PrintError("Unable to initialize COM: %x\n", hr);
		}
#endif

		g_framework->m_appPath.to_utf8(g_framework->m_appPathA);

		g_framework->_initDefaultCursors();

#ifdef BQ_WITH_GS
		g_framework->m_gss.push_back(bqGSD3D11_create());
		//g_framework->m_gss.push_back(bqGSD3D12_create());
		//g_framework->m_gss.push_back(bqGSVulkan_create());
#endif

#ifdef BQ_WITH_IMAGE
		g_framework->m_imageLoaders.push_back(bqImageLoaderDefault_create());
#endif

		g_framework->m_meshLoaders.push_back(bqMeshLoaderDefault_create());

		bqLog::PrintInfo("App path : %s\n", g_framework->m_appPathA.c_str());

		g_framework->m_vgshape_line = new bqVectorGraphicsShape;
		g_framework->m_vgshape_line->Create(4);
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
	BQ_SAFEDESTROY(m_vgshape_line);

#ifdef BQ_WITH_SOUND
	if (m_soundSystem)
	{
		bqDestroy(m_soundSystem);
		m_soundSystem = 0;
	}
#endif

	for (uint32_t i = 0; i < (uint32_t)bqCursorType::_count; ++i)
	{
		if (m_defaultCursors[i])
			delete m_defaultCursors[i];
	}

#ifdef BQ_WITH_IMAGE
	if (m_texturesForDestroy.m_size)
	{
		for (size_t i = 0; i < m_texturesForDestroy.m_size; ++i)
		{
			if (m_texturesForDestroy.m_data[i])
				delete m_texturesForDestroy.m_data[i];
		}
		m_texturesForDestroy.clear();
	}

	if (g_framework->m_imageLoaders.size())
	{
		for (auto o : g_framework->m_imageLoaders)
		{
			bqDestroy(o);
		}
		g_framework->m_imageLoaders.clear();
	}
#endif

	if (g_framework->m_meshLoaders.size())
	{
		for (auto o : g_framework->m_meshLoaders)
		{
			bqDestroy(o);
		}
		g_framework->m_meshLoaders.clear();
	}

#ifdef BQ_WITH_GUI
	if (g_framework->m_defaultFonts.m_size)
	{
		for (size_t i = 0; i < m_defaultFonts.m_size; ++i)
		{
			if (m_defaultFonts.m_data[i])
				delete m_defaultFonts.m_data[i];
		}
		m_defaultFonts.clear();
	}
	//if (g_framework->m_GUIWindows.m_head)
	//{
	//	// надо собрать все окна в массив
	//	bqArray<bqGUIWindow*> allWindows;
	//	allWindows.reserve(10);

	//	auto cw = g_framework->m_GUIWindows.m_head;
	//	auto lw = cw->m_left;

	//	while (1)
	//	{
	//		auto nw = cw->m_right;

	//		allWindows.push_back(cw->m_data);
	//		//bqFramework::Destroy(cw->m_data);

	//		if (cw == lw)
	//			break;
	//		cw = nw;
	//	}
	//	g_framework->m_GUIWindows.clear();

	//	// теперь спокойно удалять
	//	for (size_t i = 0; i < allWindows.m_size; ++i)
	//	{
	//		bqFramework::Destroy(allWindows.m_data[i]);
	//	}
	//}
	_onDestroy_GUITextDrawCallbacks();
#endif

#ifdef BQ_WITH_ARCHIVE
	_onDestroy_archive();
#endif

#ifdef BQ_WITH_GS
	if (g_framework->m_gss.size())
	{
		for (auto o : g_framework->m_gss)
		{
			o->Shutdown();
			bqDestroy(o);
		}
		g_framework->m_gss.clear();
	}
#endif

#ifdef BQ_PLATFORM_WINDOWS
	CoUninitialize();
#endif
}

void bqFramework::Update()
{
	BQ_ASSERT_STC(g_framework, "This method must be called only after framework initialization (bqFramework::Start)");

	bqInputUpdatePre();

#ifdef BQ_PLATFORM_WINDOWS
#ifdef BQ_WITH_WINDOW
	// без этого окно не будет реагировать
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#endif
#endif

	bqInputUpdatePost();

	static clock_t then = 0;
	clock_t now = clock();

	g_framework->m_deltaTime = (float)(now - then) / CLOCKS_PER_SEC;
	then = now;

	g_framework->m_GUIState.m_windowUnderCursor = 0;
}

float* bqFramework::GetDeltaTime()
{
	BQ_ASSERT_STC(g_framework, "This method must be called only after framework initialization (bqFramework::Start)");
	return &g_framework->m_deltaTime;
}

#ifdef BQ_WITH_WINDOW
bqWindow* bqFramework::CreateSystemWindow(bqWindowCallback* cb)
{
	BQ_ASSERT_STC(g_framework, "This method must be called only after framework initialization (bqFramework::Start)");
	BQ_ASSERT_STC(cb, "You need to create callback class for window");
	return new bqWindow(cb);
}
#endif

// =========== GS
#ifdef BQ_WITH_GS
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

bqGS* bqFramework::CreateGS(bqUID id)
{
	for (auto o : g_framework->m_gss)
	{
		if (CompareUIDs(o->GetUID(), id))
			return o;
	}
	return 0;
}

bqGS* bqFramework::CreateGS(const char* _name)
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

bqGS* bqFramework::CreateGS(bqUID id, const char* _name)
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
#endif

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

bqMat4* bqFramework::GetMatrixSkinned()
{
	return &g_framework->m_matrixSkinned[0];
}

uint8_t* bqFramework::CreateFileBuffer(const char* path, uint32_t* szOut, bool isText)
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
#ifdef BQ_WITH_ARCHIVE
	return bqArchiveSystem::ZipUnzip(path, szOut, 0);
#else
	return nullptr;
#endif
}

#ifdef BQ_WITH_IMAGE
uint32_t bqFramework::GetImageLoadersNum()
{
	return (uint32_t)g_framework->m_imageLoaders.size();
}

bqImageLoader* bqFramework::GetImageLoader(uint32_t i)
{
	BQ_ASSERT_ST(i < g_framework->m_imageLoaders.size());
	return g_framework->m_imageLoaders[i];
}

bqImage* bqFramework::CreateImage(const char* path)
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

				bqImage* image = il->Load(path);
				if (image)
				{
					image->m_info.m_md5 = bqCryptography::MD5(path, strlen(path));
				}

				return image;
			}
		}
	}
	return NULL;
}
#endif

#ifdef BQ_WITH_GS
bqTexture* bqFramework::CreateTexture(bqGS* gs, const char* fn, bool genMipMaps, bool linearFilter)
{
	BQ_ASSERT_ST(gs);
	BQ_ASSERT_ST(fn);
	bqTexture* newTexture = 0;

	if (gs && fn)
	{
		bqImage* img = bqFramework::CreateImage(fn);
		if (img)
		{
			bqTextureInfo ti;
			ti.m_generateMipmaps = genMipMaps;
			ti.m_filter = linearFilter ? bqTextureFilter::LLL : bqTextureFilter::PPP;
			newTexture = gs->CreateTexture(img, ti);
			delete img;
		}
	}

	return newTexture;
}
bqTextureCache* bqFramework::CreateTextureCache(bqGS* gs)
{
	BQ_ASSERT_ST(gs);
	bqTextureCache* tc = new bqTextureCache(gs);
	return tc;
}
#endif

bool bqFramework::FileExist(const char* p)
{
	BQ_ASSERT_ST(p);
	return std::filesystem::exists(p);
}

bool bqFramework::FileExist(const bqString& p)
{
	g_framework->m_fileExistString.clear();
	p.to_utf8(g_framework->m_fileExistString);
	return std::filesystem::exists(g_framework->m_fileExistString.data());
}

uint64_t bqFramework::FileSize(const char* p)
{
	BQ_ASSERT_ST(p);
	return (uint64_t)std::filesystem::file_size(p);
}

uint64_t bqFramework::FileSize(const bqString& p)
{
	g_framework->m_fileSizeString.clear();
	p.to_utf8(g_framework->m_fileSizeString);
	return (uint64_t)std::filesystem::file_size(g_framework->m_fileSizeString.data());
}

bqPolygonMesh* bqFramework::CreatePolygonMesh()
{
	return bqCreate<bqPolygonMesh>();
}

uint32_t bqFramework::GetMeshLoadersNum()
{
	return (uint32_t)g_framework->m_meshLoaders.size();
}

bqMeshLoader* bqFramework::GetMeshLoader(uint32_t i)
{
	BQ_ASSERT_ST(i < g_framework->m_meshLoaders.size());
	return g_framework->m_meshLoaders[i];
}

void bqFramework::CreateMesh(const char* path, bqMeshLoaderCallback* cb)
{
	BQ_ASSERT_ST(path);
	bqStringA stra;
	std::filesystem::path p = path;
	auto e = p.extension();
	uint32_t mln = GetMeshLoadersNum();
	for (uint32_t i = 0; i < mln; ++i)
	{
		auto ml = GetMeshLoader(i);
		auto sfc = ml->GetSupportedFilesCount();
		for (uint32_t o = 0; o < sfc; ++o)
		{
			bqString sfe = ml->GetSupportedFileExtension(o);
			sfe.insert(U".", 0);
			sfe.to_lower();
			sfe.to_utf8(stra);
			auto stre = lowercase(e.generic_string());
			if (strcmp((const char*)stra.data(), stre.c_str()) == 0)
			{
				ml->Load(path, cb);
				return;
			}
		}
	}
}

class bqFramework_defaultMeshLoadCallback : public bqMeshLoaderCallback
{
	bqArray<bqMesh*>* m_a = 0;
public:
	bqFramework_defaultMeshLoadCallback(bqArray<bqMesh*>* a)
	:
		m_a(a)
	{}
	virtual ~bqFramework_defaultMeshLoadCallback() {}
	virtual void OnMesh(bqMesh* newMesh, bqString* n, bqString* mn)
	{
		if (newMesh)
		{
			bqMeshLoaderCallback::OnMeshLog(newMesh, n, mn);
			m_a->push_back(newMesh);
		}
	}
};

bqMesh* bqFramework::CreateMesh(const char* path)
{
	BQ_ASSERT_ST(path);
	bqMesh* r = 0;

	bqArray<bqMesh*> a;
	bqFramework_defaultMeshLoadCallback cb(&a);
	CreateMesh(path, &cb);

	if (a.size())
	{
		r = a.m_data[0];

		for (size_t i = 1; i < a.m_size; ++i)
		{
			delete a.m_data[i];
		}
	}

	return r;
}

void bqFramework::CreateMesh(const char* p, bqArray<bqMesh*>* a)
{
	BQ_ASSERT_ST(p);
	BQ_ASSERT_ST(a);

	a->clear();
	bqFramework_defaultMeshLoadCallback cb(a);
	CreateMesh(p, &cb);
}

bqString bqFramework::GetAppPath()
{
	return g_framework->m_appPath;
}

bqStringA bqFramework::GetAppPathA()
{
	return g_framework->m_appPathA;
}


bqStringA bqFramework::GetPath(const bqString& v)
{
	bqString p = g_framework->m_appPath;
	p.append(v);
	bqStringA stra;
	p.to_utf8(stra);

	if (!std::filesystem::exists(stra.c_str()))
	{
		p.assign(v);

		while (p.size())
		{
			if (p[0] == U'.'
				|| p[0] == U'\\'
				|| p[0] == U'/')
				p.pop_front();
			else
				break;
		}

		p.to_utf8(stra);
	}

	return stra;
}

#ifdef BQ_WITH_GUI
bqGUIFont* bqFramework::GetDefaultFont(bqGUIDefaultFont t)
{
	if ((uint32_t)t >= g_framework->m_defaultFonts.m_size)
		return g_framework->m_defaultFonts.m_data[0];

	switch (t)
	{
	case bqGUIDefaultFont::Text:
	case bqGUIDefaultFont::Icons:
		return g_framework->m_defaultFonts.m_data[(uint32_t)t];
		break;
	default:
		bqLog::PrintWarning("%s : not implemented\n", BQ_FUNCTION);
		break;
	}
	return g_framework->m_defaultFonts.m_data[0];
}

void bqFramework::InitDefaultFonts(bqGS* gs)
{
	static bool isInit = false;
	if (!isInit)
	{
		auto getImage = [](uint8_t* buf, uint32_t sz)->bqImage* {
			for (uint32_t i = 0; i < bqFramework::GetImageLoadersNum(); ++i)
			{
				auto il = bqFramework::GetImageLoader(i);
				for (uint32_t o = 0; o < il->GetSupportedFilesCount(); ++o)
				{
					auto str = il->GetSupportedFileExtension(o);
					if (str == U"png")
						return il->Load("something/file.png", buf, sz);
				}
			}
			return 0;
		};

		auto getTexture = [gs](bqImage* img)->bqTexture* {
			bqTextureInfo ti;
			bqTexture* t = gs->CreateTexture(img, ti);
			if (img)
				delete img;
			return t;
		};

		bqImage* img = getImage(g_defaultFontPNG, sizeof(g_defaultFontPNG));

		if (!img)
			return;

		bqTexture* myFontTexture = getTexture(img);

		if (!myFontTexture)
			return;

		g_framework->m_texturesForDestroy.push_back(myFontTexture);

		bqGUIFont* myFont = bqFramework::CreateGUIFont();
	//	myFont->m_characterSpacing = 3.f;
		myFont->AddTexture(myFontTexture);
		myFont->AddGlyph(U'A', bqVec2f(0, 0), bqPoint(11, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'B', bqVec2f(10, 0), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'C', bqVec2f(19, 0), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'D', bqVec2f(27, 0), bqPoint(11, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'E', bqVec2f(37, 0), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'F', bqVec2f(45, 0), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'G', bqVec2f(52, 0), bqPoint(11, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'H', bqVec2f(62, 0), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'I', bqVec2f(72, 0), bqPoint(4, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'J', bqVec2f(76, 0), bqPoint(6, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'K', bqVec2f(82, 0), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'L', bqVec2f(90, 0), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'M', bqVec2f(97, 0), bqPoint(13, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'N', bqVec2f(110, 0), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'O', bqVec2f(120, 0), bqPoint(11, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'P', bqVec2f(131, 0), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Q', bqVec2f(139, 0), bqPoint(12, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'R', bqVec2f(151, 0), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'S', bqVec2f(159, 0), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'T', bqVec2f(166, 0), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'U', bqVec2f(175, 0), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'V', bqVec2f(184, 0), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'W', bqVec2f(193, 0), bqPoint(14, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'X', bqVec2f(207, 0), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Y', bqVec2f(216, 0), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Z', bqVec2f(224, 0), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'0', bqVec2f(231, 0), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'1', bqVec2f(239, 0), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'2', bqVec2f(247, 0), bqPoint(7, 16), 0, bqPoint(256, 256));

		myFont->AddGlyph(U'3', bqVec2f(1, 20), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'4', bqVec2f(9, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'5', bqVec2f(17, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'6', bqVec2f(25, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'7', bqVec2f(33, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'8', bqVec2f(41, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'9', bqVec2f(49, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'a', bqVec2f(57, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'b', bqVec2f(65, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'c', bqVec2f(73, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'd', bqVec2f(80, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'e', bqVec2f(89, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'f', bqVec2f(96, 20), bqPoint(6, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'g', bqVec2f(102, 20), bqPoint(6, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'h', bqVec2f(109, 20), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'i', bqVec2f(117, 20), bqPoint(3, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'j', bqVec2f(120, 20), bqPoint(4, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'k', bqVec2f(125, 20), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'l', bqVec2f(132, 20), bqPoint(3, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'm', bqVec2f(136, 20), bqPoint(12, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'n', bqVec2f(148, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'o', bqVec2f(157, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'p', bqVec2f(165, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'q', bqVec2f(173, 20), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'r', bqVec2f(182, 20), bqPoint(6, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U's', bqVec2f(188, 20), bqPoint(5, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U't', bqVec2f(194, 20), bqPoint(5, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'u', bqVec2f(199, 20), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'v', bqVec2f(207, 20), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'w', bqVec2f(214, 20), bqPoint(12, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'x', bqVec2f(226, 20), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'y', bqVec2f(233, 20), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'z', bqVec2f(240, 20), bqPoint(7, 16), 0, bqPoint(256, 256));

		myFont->AddGlyph(U'~', bqVec2f(1, 39), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'`', bqVec2f(9, 39), bqPoint(4, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'!', bqVec2f(14, 39), bqPoint(4, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'?', bqVec2f(19, 39), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'\'', bqVec2f(26, 39), bqPoint(4, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'\"', bqVec2f(30, 39), bqPoint(5, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'@', bqVec2f(37, 39), bqPoint(13, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'#', bqVec2f(50, 39), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'№', bqVec2f(59, 39), bqPoint(15, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U';', bqVec2f(75, 39), bqPoint(3, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U':', bqVec2f(80, 39), bqPoint(3, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'%', bqVec2f(84, 39), bqPoint(11, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'&', bqVec2f(95, 39), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'^', bqVec2f(106, 39), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'<', bqVec2f(114, 39), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'>', bqVec2f(121, 39), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'(', bqVec2f(130, 39), bqPoint(5, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U')', bqVec2f(134, 39), bqPoint(5, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'[', bqVec2f(140, 39), bqPoint(5, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U']', bqVec2f(144, 39), bqPoint(5, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'{', bqVec2f(150, 39), bqPoint(5, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'}', bqVec2f(154, 39), bqPoint(5, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'*', bqVec2f(160, 39), bqPoint(6, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'.', bqVec2f(167, 39), bqPoint(3, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U',', bqVec2f(171, 39), bqPoint(3, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'/', bqVec2f(174, 39), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'\\', bqVec2f(181, 39), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'|', bqVec2f(189, 39), bqPoint(4, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'+', bqVec2f(195, 39), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'-', bqVec2f(203, 39), bqPoint(5, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'=', bqVec2f(208, 39), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'_', bqVec2f(216, 39), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'$', bqVec2f(233, 39), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U' ', bqVec2f(225, 39), bqPoint(8, 16), 0, bqPoint(256, 256));

		myFont->AddGlyph(U'А', bqVec2f(0, 59), bqPoint(11, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Б', bqVec2f(10, 59), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'В', bqVec2f(19, 59), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Г', bqVec2f(28, 59), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Д', bqVec2f(37, 59), bqPoint(12, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Е', bqVec2f(48, 59), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Ё', bqVec2f(56, 59), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Ж', bqVec2f(64, 59), bqPoint(13, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'З', bqVec2f(77, 59), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'И', bqVec2f(85, 59), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Й', bqVec2f(95, 59), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'К', bqVec2f(105, 59), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Л', bqVec2f(114, 59), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'М', bqVec2f(124, 59), bqPoint(12, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Н', bqVec2f(138, 59), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'О', bqVec2f(147, 59), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'П', bqVec2f(159, 59), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Р', bqVec2f(168, 59), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'С', bqVec2f(176, 59), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Т', bqVec2f(184, 59), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'У', bqVec2f(193, 59), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Ф', bqVec2f(201, 59), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Х', bqVec2f(211, 59), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Ц', bqVec2f(220, 59), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Ч', bqVec2f(230, 59), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Ш', bqVec2f(239, 59), bqPoint(12, 16), 0, bqPoint(256, 256));

		myFont->AddGlyph(U'Щ', bqVec2f(2, 79), bqPoint(13, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Ь', bqVec2f(16, 79), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Ы', bqVec2f(25, 79), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Ъ', bqVec2f(36, 79), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Э', bqVec2f(46, 79), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Ю', bqVec2f(55, 79), bqPoint(13, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'Я', bqVec2f(69, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'а', bqVec2f(78, 79), bqPoint(6, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'б', bqVec2f(86, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'в', bqVec2f(94, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'г', bqVec2f(101, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'д', bqVec2f(110, 79), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'е', bqVec2f(120, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'ё', bqVec2f(128, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'ж', bqVec2f(135, 79), bqPoint(11, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'з', bqVec2f(146, 79), bqPoint(6, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'и', bqVec2f(154, 79), bqPoint(6, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'й', bqVec2f(162, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'к', bqVec2f(171, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'л', bqVec2f(178, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'м', bqVec2f(186, 79), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'н', bqVec2f(197, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'о', bqVec2f(205, 79), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'п', bqVec2f(214, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'р', bqVec2f(223, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'с', bqVec2f(230, 79), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'т', bqVec2f(237, 79), bqPoint(6, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'у', bqVec2f(247, 79), bqPoint(7, 16), 0, bqPoint(256, 256));

		myFont->AddGlyph(U'ф', bqVec2f(1, 98), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'х', bqVec2f(11, 98), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'ц', bqVec2f(18, 98), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'ч', bqVec2f(26, 98), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'ш', bqVec2f(35, 98), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'щ', bqVec2f(46, 98), bqPoint(11, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'ь', bqVec2f(58, 98), bqPoint(7, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'ы', bqVec2f(66, 98), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'ъ', bqVec2f(75, 98), bqPoint(9, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'э', bqVec2f(84, 98), bqPoint(8, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'ю', bqVec2f(92, 98), bqPoint(10, 16), 0, bqPoint(256, 256));
		myFont->AddGlyph(U'я', bqVec2f(103, 98), bqPoint(7, 16), 0, bqPoint(256, 256));

		g_framework->m_defaultFonts.push_back(myFont);

		img = getImage(g_defaultIconsPNG, sizeof(g_defaultIconsPNG));
		if (img)
		{
			myFontTexture = getTexture(img);
			if (myFontTexture)
			{
				g_framework->m_texturesForDestroy.push_back(myFontTexture);

				myFont = bqFramework::CreateGUIFont();
				myFont->AddTexture(myFontTexture);
				myFont->AddGlyph((uint32_t)bqGUIDefaultIconID::CheckboxUncheck, bqVec2f(0, 0), bqPoint(14, 14), 0, bqPoint(512, 512));
				myFont->AddGlyph((uint32_t)bqGUIDefaultIconID::RadioUncheck, bqVec2f(0, 0), bqPoint(14, 14), 0, bqPoint(512, 512));
				myFont->AddGlyph((uint32_t)bqGUIDefaultIconID::CheckboxCheck, bqVec2f(28, 0), bqPoint(14, 14), 0, bqPoint(512, 512));
				myFont->AddGlyph((uint32_t)bqGUIDefaultIconID::RadioCheck, bqVec2f(14, 0), bqPoint(14, 14), 0, bqPoint(512, 512));
				myFont->AddGlyph((uint32_t)bqGUIDefaultIconID::Minus, bqVec2f(43, 0), bqPoint(14, 14), 0, bqPoint(512, 512));
				myFont->AddGlyph((uint32_t)bqGUIDefaultIconID::Plus, bqVec2f(57, 0), bqPoint(14, 14), 0, bqPoint(512, 512));
				myFont->AddGlyph((uint32_t)bqGUIDefaultIconID::ArrowUp, bqVec2f(71, 0), bqPoint(14, 14), 0, bqPoint(512, 512));
				myFont->AddGlyph((uint32_t)bqGUIDefaultIconID::ArrowDown, bqVec2f(85, 0), bqPoint(14, 14), 0, bqPoint(512, 512));
				myFont->AddGlyph((uint32_t)bqGUIDefaultIconID::ArrowRight, bqVec2f(97, 0), bqPoint(14, 14), 0, bqPoint(512, 512));
				myFont->AddGlyph((uint32_t)bqGUIDefaultIconID::ArrowLeft, bqVec2f(109, 0), bqPoint(14, 14), 0, bqPoint(512, 512));
				myFont->AddGlyph((uint32_t)bqGUIDefaultIconID::CloseWindow, bqVec2f(125, 0), bqPoint(14, 14), 0, bqPoint(512, 512));

				g_framework->m_defaultFonts.push_back(myFont);
			}
		}

		isInit = true;
	}

	g_framework->m_themeDark.m_windowActiveTitleTextFont = bqFramework::GetDefaultFont(bqGUIDefaultFont::Text);
	g_framework->m_themeDark.m_windowNActiveTitleTextFont = bqFramework::GetDefaultFont(bqGUIDefaultFont::Text);
	g_framework->m_themeLight.m_windowActiveTitleTextFont = g_framework->m_themeDark.m_windowActiveTitleTextFont;
	g_framework->m_themeLight.m_windowNActiveTitleTextFont = g_framework->m_themeDark.m_windowNActiveTitleTextFont;
	
	g_framework->m_themeDark.m_staticTextFont = bqFramework::GetDefaultFont(bqGUIDefaultFont::Text);
	g_framework->m_themeLight.m_staticTextFont = g_framework->m_themeDark.m_staticTextFont;

	g_framework->m_themeDark.m_buttonTextFont = bqFramework::GetDefaultFont(bqGUIDefaultFont::Text);
	g_framework->m_themeLight.m_buttonTextFont = g_framework->m_themeDark.m_buttonTextFont;
}

bqGUIFont* bqFramework::CreateGUIFont()
{
	return new bqGUIFont();
}

bqGUIStyle* bqFramework::GetGUIStyle(bqGUIStyleTheme theme)
{
	switch (theme)
	{
	case bqGUIStyleTheme::Light:
		return &g_framework->m_themeLight;
	case bqGUIStyleTheme::Dark:
		return &g_framework->m_themeDark;
	}
	return &g_framework->m_themeLight;
}

void bqFrameworkImpl::_initGUIThemes()
{
	bqLog::PrintInfo("Init GUI Themes\n");
	g_framework->m_themeLight.m_windowActiveBGColor1 = 0xF7F7F7;
	g_framework->m_themeLight.m_windowActiveBGColor2 = 0xF7F7F7;
	g_framework->m_themeLight.m_windowActiveBorderColor = 0xE1E6F7;
	g_framework->m_themeLight.m_windowActiveTitleBGColor1 = 0x64A1E4;
	g_framework->m_themeLight.m_windowActiveTitleBGColor2 = 0x64A1E4;
	g_framework->m_themeLight.m_windowActiveTitleTextColor = 0xF8FAFC;

	g_framework->m_themeLight.m_windowNActiveBGColor1 = 0xE7E7E7;
	g_framework->m_themeLight.m_windowNActiveBGColor2 = 0xE7E7E7;
	g_framework->m_themeLight.m_windowNActiveBorderColor = 0xE1E6F7;
	g_framework->m_themeLight.m_windowNActiveTitleBGColor1 = 0xD0D0D0;
	g_framework->m_themeLight.m_windowNActiveTitleBGColor2 = 0xD0D0D0;
	g_framework->m_themeLight.m_windowNActiveTitleTextColor = 0x0;

	g_framework->m_themeLight.m_buttonBGColor1 = 0x999999;
	g_framework->m_themeLight.m_buttonBGColor2 = 0x666666;
	g_framework->m_themeLight.m_buttonBorderColor = 0x999999;
	g_framework->m_themeLight.m_buttonTextColor = 0xE7E7E7;
	g_framework->m_themeLight.m_buttonDisabledBGColor1 = 0x999999;
	g_framework->m_themeLight.m_buttonDisabledBGColor2 = 0x666666;
	g_framework->m_themeLight.m_buttonDisabledBorderColor = 0x999999;
	g_framework->m_themeLight.m_buttonDisabledTextColor = 0x555555;
	g_framework->m_themeLight.m_buttonMouseHoverBGColor1 = 0xAAAAAA;
	g_framework->m_themeLight.m_buttonMouseHoverBGColor2 = 0x777777;
	g_framework->m_themeLight.m_buttonMouseHoverTextColor = 0x222222;
	g_framework->m_themeLight.m_buttonMousePressBGColor1 = 0x777777;
	g_framework->m_themeLight.m_buttonMousePressBGColor2 = 0x444444;
	g_framework->m_themeLight.m_buttonMousePressTextColor = 0xFF0000;

	g_framework->m_themeLight.m_chkradioDisabledTextColor = 0xFF0000;
	g_framework->m_themeLight.m_chkradioMouseHoverTextColor = bq::ColorYellow;
	g_framework->m_themeLight.m_chkradioMousePressTextColor = 0xFF55FF;
	g_framework->m_themeLight.m_chkradioTextColor = 0x0;

	g_framework->m_themeLight.m_textEditorBGColor = 0xFFFFFF;
	g_framework->m_themeLight.m_textEditorLine1BGColor = 0xFAFAFA;
	g_framework->m_themeLight.m_textEditorLine2BGColor = 0xF1F1F1;
	g_framework->m_themeLight.m_textEditorTextColor = 0x0;
	g_framework->m_themeLight.m_textEditorSelectedTextBGColor = 0x999999;
	g_framework->m_themeLight.m_textEditorSelectedTextColor = 0x0;
	g_framework->m_themeLight.m_textEditorCursorColor = 0x0;

	g_framework->m_themeLight.m_staticTextBGColor = 0xFAFAFA;
	g_framework->m_themeLight.m_staticTextTextColor = 0;

	g_framework->m_themeLight.m_listboxBGColor = 0xFAFAFA;
	g_framework->m_themeLight.m_listboxLine1BGColor = 0xFAFAFA;
	g_framework->m_themeLight.m_listboxLine2BGColor = 0xF1F1F1;
	g_framework->m_themeLight.m_listboxSelectedLineBGColor = 0x999999;

	g_framework->m_themeLight.m_sliderTextColor = 0;
	g_framework->m_themeLight.m_sliderAxisEmtpyColor = 0xD1D1D1;
	g_framework->m_themeLight.m_sliderAxisFillColor = bq::ColorBlue;
	g_framework->m_themeLight.m_sliderControlColor = 0xD1D1D1;

	g_framework->m_themeLight.m_scrollbarBGColor = 0xE9E9E9;
	g_framework->m_themeLight.m_scrollbarControlColor = 0xC6C6C6;
	g_framework->m_themeLight.m_scrollbarControlMouseHoverColor = 0xA6A6A6;
	g_framework->m_themeLight.m_scrollbarControlDragColor = 0xA6A6A6;
	
	g_framework->m_themeLight.m_pictureBoxBGColor = 0xFFFFFF;

	g_framework->m_themeDark = g_framework->m_themeLight;
}

//void bqFramework::UpdateGUI()
//{
	//if (g_framework->m_GUIWindows.m_head)
	//{
	//	g_framework->m_GUIState.m_scrollBlock = false;
	//	
	//	// сброс значения здеь, оно будет установлено в каком нибудь Update если курсор попадает в его область
	//	g_framework->m_GUIState.m_windowUnderCursor = 0;

	//	auto last = g_framework->m_GUIWindows.m_head;
	//	auto curr = last->m_left;
	//	while (1)
	//	{
	//		if (curr->m_data->IsVisible() 
	//			&& 
	//			!g_framework->m_GUIState.m_windowUnderCursor // запрет обрабатывать ввод другим окнам
	//			)
	//		{
	//			curr->m_data->Update();
	//		}

	//		if (curr == last)
	//			break;
	//		curr = curr->m_left;
	//	}
	//}
//}

//void bqFramework::DrawGUI(bqGS* gs)
//{
	/*if (g_framework->m_GUIWindows.m_head)
	{
		auto last = g_framework->m_GUIWindows.m_head;
		auto curr = last->m_left;
		while (1)
		{
			if (curr->m_data->IsVisible())
			{
				curr->m_data->Draw(gs, g_framework->m_deltaTime);
			}

			if (curr == last)
				break;
			curr = curr->m_left;
		}
	}*/
//}

//void bqFramework::RebuildGUI()
//{
	/*if (g_framework->m_GUIWindows.m_head)
	{
		auto last = g_framework->m_GUIWindows.m_head;
		auto curr = last->m_left;
		while (1)
		{
			curr->m_data->Rebuild();

			if (curr == last)
				break;
			curr = curr->m_left;
		}
	}*/
//}

//void DestroyGUIElement_internal(bqGUIElement* e)
//{
//	if (e->GetChildren()->m_head)
//	{
//		auto children = e->GetChildren();
//		if (children->m_head)
//		{
//			auto curr = children->m_head;
//			auto last = curr->m_left;
//			while (1)
//			{
//				DestroyGUIElement_internal(dynamic_cast<bqGUIElement*>(curr->m_data));
//				if (curr == last)
//					break;
//				curr = curr->m_right;
//			}
//		}
//	}
//
//	delete e;
//}
//
//void _DestroyGUIElement(bqGUIElement* e)
//{
//	e->SetParent(0);
//	DestroyGUIElement_internal(e);
//}

void bqFramework::Destroy(bqGUIWindow* )
{
	/*BQ_ASSERT_ST(w);
	_DestroyGUIElement(w->m_rootElement);
	g_framework->m_GUIWindows.erase_first(w);
	delete w;*/
}

void bqFramework::Destroy(bqGUIElement* )
{
	/*BQ_ASSERT_ST(e);
	if (e->GetWindow()->m_rootElement == e)
		return;
	_DestroyGUIElement(e);*/
}

void bqFrameworkImpl::_initGUITextDrawCallbacks()
{
	bqLog::PrintInfo("Init GUI callbacks\n");

	m_defaultTextDrawCallback_button = new bqGUIButtonTextDrawCallback;
	//m_defaultTextDrawCallback_icons = new bqGUICheckRadioBoxTextDrawCallback;
	//m_defaultTextDrawCallback_textEditor = new bqGUITextEditorTextDrawCallback;
	//m_defaultTextDrawCallback_listbox = new bqGUIListBoxTextDrawCallback;


	//// далее коллбеки работающие чуть по иному
	//// в таких коллбэках нужно будет указывать m_element, внутри этого элемента
	//// это позволяет использовать m_style - темы
	//// надо инициилизировать шрифты - в том месте где шрифты создаются bqFramework::InitDefaultFonts
	//// надо переделать остальные коллбэки.
	//// При Rebuild и Draw надо указать
	//// m_textDrawCallback->m_element = this;
	m_defaultTextDrawCallback_staticText = new bqGUIStaticTextTextDrawCallback;
	m_defaultTextDrawCallback_window = new bqGUIWindowTextDrawCallback;
	
//	m_defaultTextDrawCallback_slider = new bqGUISliderTextDrawCallback;
}

void bqFrameworkImpl::_onDestroy_GUITextDrawCallbacks()
{
	delete m_defaultTextDrawCallback_window; m_defaultTextDrawCallback_window = 0;
	//delete m_defaultTextDrawCallback_button; m_defaultTextDrawCallback_button = 0;
	//delete m_defaultTextDrawCallback_icons; m_defaultTextDrawCallback_icons = 0;
	//delete m_defaultTextDrawCallback_textEditor; m_defaultTextDrawCallback_textEditor = 0;
	//delete m_defaultTextDrawCallback_listbox; m_defaultTextDrawCallback_listbox = 0;
//	delete m_defaultTextDrawCallback_slider; m_defaultTextDrawCallback_slider = 0;
	delete m_defaultTextDrawCallback_staticText; m_defaultTextDrawCallback_staticText = 0;
}

const bqGUIState& bqFramework::GetGUIState()
{
	return g_framework->m_GUIState;
}


#endif

bqCursor* bqFramework::CreateCursor(const char* fn)
{
	bqCursor* newCursor = 0;

#ifdef BQ_PLATFORM_WINDOWS
	bqCursorWin32* c = new bqCursorWin32;
	bqCursorWin32* ptr = (bqCursorWin32*)c;
	ptr->m_handle = (HCURSOR)::LoadImageA(GetModuleHandle(0), fn, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
#endif

	return newCursor;
}

void bqFrameworkImpl::_initDefaultCursors()
{
#ifdef BQ_PLATFORM_WINDOWS
	for (uint32_t i = 0; i < (uint32_t)bqCursorType::_count; ++i)
	{
		m_defaultCursors[i] = new bqCursorWin32();

		switch ((bqCursorType)i)
		{
		default:
		case bqCursorType::Arrow: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_ARROW); break;
		case bqCursorType::Cross: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_CROSS); break;
		case bqCursorType::Hand: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_HAND); break;
		case bqCursorType::Help: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_HELP); break;
		case bqCursorType::IBeam: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_IBEAM); break;
		case bqCursorType::No: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_NO); break;
		case bqCursorType::Size: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_SIZEALL); break;
		case bqCursorType::SizeNESW: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_SIZENESW); break;
		case bqCursorType::SizeNS: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_SIZENS); break;
		case bqCursorType::SizeNWSE: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_SIZENWSE); break;
		case bqCursorType::SizeWE: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_SIZEWE); break;
		case bqCursorType::UpArrow: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_UPARROW); break;
		case bqCursorType::Wait: ((bqCursorWin32*)m_defaultCursors[i])->m_handle = LoadCursor(0, IDC_WAIT); break;
		}
	}
#endif

	m_activeCursor = m_defaultCursors[0];
}

bqCursor* bqFramework::GetDefaultCursor(bqCursorType ct)
{
	BQ_ASSERT_ST(ct != bqCursorType::_count);
	return g_framework->m_defaultCursors[(uint32_t)ct];
}

bqCursor* bqFramework::GetActiveCursor()
{
	return g_framework->m_activeCursor;
}

void bqFramework::SetActiveCursor(bqCursor* c)
{
	g_framework->m_activeCursor = c;	
}

#ifdef BQ_WITH_SOUND
bqSoundSystem* bqFramework::GetSoundSystem()
{
	if (!g_framework->m_soundSystem)
	{
		g_framework->m_soundSystem = bqCreate<bqSoundSystemImpl>();
		if (!g_framework->m_soundSystem->Init())
		{
			bqDestroy(g_framework->m_soundSystem);
			g_framework->m_soundSystem = 0;
		}
	}

	return g_framework->m_soundSystem;
}
#endif

#ifdef BQ_WITH_PHYSICS
bqPhysics* bqFramework::GetPhysicsSystem()
{
	if (!g_framework->m_physics)
	{
		g_framework->m_physics = new bqPhysics();
	}

	return g_framework->m_physics;
}
#endif

bqVec2i bqFramework::GetDesktopWindowSize()
{
	bqVec2i v;
#ifdef BQ_WITH_WINDOW
	RECT r;
	GetWindowRect(GetDesktopWindow(), &r);
	v.x = r.right - r.left;
	v.y = r.bottom - r.top;
#endif
	return v;
}

void bqFramework::ShowCursor(bool v)
{
#ifdef BQ_WITH_WINDOW
	::ShowCursor((BOOL)v);
#endif
}

const bqStringA& bqFramework::GetUTF8String(const bqString& s)
{
	g_framework->m_UTF8String.clear();
	s.to_utf8(g_framework->m_UTF8String);
	return g_framework->m_UTF8String;
}

const bqStringW& bqFramework::GetUTF16String(const bqString& s)
{
	g_framework->m_UTF16String.clear();
	s.to_utf16(g_framework->m_UTF16String);
	return g_framework->m_UTF16String;
}

#ifdef BQ_WITH_POPUP
bqPopup* bqFramework::CreatePopup()
{
	bqPopupWin32* p = new bqPopupWin32();
	return p;
}
void bqFramework::ShowPopupAtCursor(bqPopup* p, bqWindow* w)
{
	BQ_ASSERT_ST(p);
	BQ_ASSERT_ST(w);

	p->Show(w, (uint32_t)g_framework->m_input.m_mousePosition.x, (uint32_t)g_framework->m_input.m_mousePosition.y);
}
#endif

void bqFramework::ClipCursor(bqVec4f* r)
{
	BQ_ASSERT_ST(r);
#ifdef BQ_PLATFORM_WINDOWS
	if (r)
	{
		RECT rct;
		rct.left = r->x;
		rct.top = r->y;
		rct.right = r->z;
		rct.bottom = r->w;
		::ClipCursor(&rct);
	}
	else
	{
		::ClipCursor(0);
	}
#endif
}

#ifdef BQ_PLATFORM_WINDOWS
bqGUIIconTexture_GDI* bqFramework::CreateGUIIconTexture_GDI(bqImage* img)
{
	BQ_ASSERT_ST(img);

	if (img)
	{
		bqGUIIconTexture_GDI* t = new bqGUIIconTexture_GDI;
		if(t->_create(img))
			return t;
	}

	return 0;
}
#endif

