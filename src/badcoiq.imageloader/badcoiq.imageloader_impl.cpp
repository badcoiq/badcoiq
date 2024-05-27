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

#include "badcoiq.imageloader.h"
#ifdef BQ_WITH_IMAGE

const char* g_badcoiqIMGLoaderFileExt[] =
{
#ifdef BQ_WITH_IMAGE_BMP
	"bmp",
#endif

#ifdef BQ_WITH_IMAGE_PNG
	"png",
#endif

#ifdef BQ_WITH_IMAGE_JPG
	"jpg",
	"jpeg",
	"jfif",
#endif

#ifdef BQ_WITH_IMAGE_TGA
	"tga",
#endif
};

const char* g_badcoiqIMGLoaderFileName[] =
{
#ifdef BQ_WITH_IMAGE_BMP
	"Windows Bitmap",
#endif

#ifdef BQ_WITH_IMAGE_PNG
	"Portable Network Graphics",
#endif

#ifdef BQ_WITH_IMAGE_JPG
	"JPEG",
	"JPEG",
	"JPEG",
#endif

#ifdef BQ_WITH_IMAGE_TGA
	"Truevision TGA",
#endif
};


bqImageLoaderImpl::bqImageLoaderImpl() 
{
	bqLog::PrintInfo("Init image loader. File formats : ");

	uint32_t fn = GetSupportedFilesCount();
	for (uint32_t i = 0; i < fn; ++i)
	{
		bqLog::Print("%s ", g_badcoiqIMGLoaderFileExt[i]);
	}
	bqLog::Print("\n");
}
bqImageLoaderImpl::~bqImageLoaderImpl() {}


uint32_t bqImageLoaderImpl::GetSupportedFilesCount()
{
	return sizeof(g_badcoiqIMGLoaderFileExt) / sizeof(char*);
}

bqString bqImageLoaderImpl::GetSupportedFileExtension(uint32_t i)
{
	return g_badcoiqIMGLoaderFileExt[i];
}

bqString bqImageLoaderImpl::GetSupportedFileName(uint32_t i)
{
	return g_badcoiqIMGLoaderFileName[i];
}

bqImageLoaderImpl::extension bqImageLoaderImpl::_GetExtension(const char* path)
{
	size_t len = strlen(path);
	if (len > 4)
	{
		// find last '.'
		size_t last_dot = 0;
		for (size_t i = 0; i < len; ++i)
		{
			if (path[i] == '.')
				last_dot = i;
		}
		if (last_dot)
		{
#ifdef BQ_WITH_IMAGE_BMP
			if (strcmp(".bmp", &path[last_dot]) == 0)
				return bqImageLoaderImpl::extension::bmp;
#endif
#ifdef BQ_WITH_IMAGE_PNG
			if (strcmp(".png", &path[last_dot]) == 0)
				return bqImageLoaderImpl::extension::png;
#endif
#ifdef BQ_WITH_IMAGE_TGA
			if (strcmp(".tga", &path[last_dot]) == 0)
				return bqImageLoaderImpl::extension::tga;
#endif
#ifdef BQ_WITH_IMAGE_JPG
			if (strcmp(".jpeg", &path[last_dot]) == 0)
				return bqImageLoaderImpl::extension::jpg;
			if (strcmp(".jpg", &path[last_dot]) == 0)
				return bqImageLoaderImpl::extension::jpg;
			if (strcmp(".jfif", &path[last_dot]) == 0)
				return bqImageLoaderImpl::extension::jpg;
#endif
		}
	}
	return extension::_bad;
}

bqImage* bqImageLoaderImpl::Load(const char* path)
{
	auto e = _GetExtension(path);
	switch (e)
	{
	case bqImageLoaderImpl::extension::_bad:
		break;
#ifdef BQ_WITH_IMAGE_BMP
	case bqImageLoaderImpl::extension::bmp:
		return LoadBMP(path);
#endif
#ifdef BQ_WITH_IMAGE_JPG
	case bqImageLoaderImpl::extension::jpg:
		return LoadJPG(path);
#endif
#ifdef BQ_WITH_IMAGE_PNG
	case bqImageLoaderImpl::extension::png:
		return LoadPNG(path);
#endif
#ifdef BQ_WITH_IMAGE_TGA
	case bqImageLoaderImpl::extension::tga:
		return LoadTGA(path);
#endif
	}
	return nullptr;
}

bqImage* bqImageLoaderImpl::Load(const char* path, uint8_t* buffer, uint32_t bufferSz)
{
	auto e = _GetExtension(path);
	switch (e)
	{
	case bqImageLoaderImpl::extension::_bad:
		break;
#ifdef BQ_WITH_IMAGE_BMP
	case bqImageLoaderImpl::extension::bmp:
		return LoadBMP(path, buffer, bufferSz);
#endif
#ifdef BQ_WITH_IMAGE_JPG
	case bqImageLoaderImpl::extension::jpg:
		return LoadJPG(path, buffer, bufferSz);
#endif
#ifdef BQ_WITH_IMAGE_PNG
	case bqImageLoaderImpl::extension::png:
		return LoadPNG(path, buffer, bufferSz);
#endif
#ifdef BQ_WITH_IMAGE_TGA
	case bqImageLoaderImpl::extension::tga:
		return LoadTGA(path, buffer, bufferSz);
#endif
	}
	return nullptr;
}

#endif
