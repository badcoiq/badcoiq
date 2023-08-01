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

#include "badcoiq.imageloader.h"

bqImageLoaderImpl::bqImageLoaderImpl() {}
bqImageLoaderImpl::~bqImageLoaderImpl() {}

uint32_t bqImageLoaderImpl::GetSupportedFilesCount()
{
	return 6;
}

bqString bqImageLoaderImpl::GetSupportedFileExtension(uint32_t i)
{
	switch (i)
	{
	case 0:
		return "bmp";
	case 1:
		return "png";
	case 2:
		return "jpg";
	case 3:
		return "jpeg";
	case 4:
		return "jfif";
	case 5:
		return "tga";
	}
	return "-";
}

bqString bqImageLoaderImpl::GetSupportedFileName(uint32_t i)
{
	switch (i)
	{
	case 0:
		return "Windows Bitmap";
	case 1:
		return "Portable Network Graphics";
	case 2:
	case 3:
	case 4:
		return "JPEG";
	case 5:
		return "Truevision TGA";
	}
	return "-";
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
			if (strcmp(".bmp", &path[last_dot]) == 0)
				return bqImageLoaderImpl::extension::bmp;
			if (strcmp(".png", &path[last_dot]) == 0)
				return bqImageLoaderImpl::extension::png;
			if (strcmp(".tga", &path[last_dot]) == 0)
				return bqImageLoaderImpl::extension::tga;
			if (strcmp(".jpeg", &path[last_dot]) == 0)
				return bqImageLoaderImpl::extension::jpg;
			if (strcmp(".jpg", &path[last_dot]) == 0)
				return bqImageLoaderImpl::extension::jpg;
			if (strcmp(".jfif", &path[last_dot]) == 0)
				return bqImageLoaderImpl::extension::jpg;
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
	case bqImageLoaderImpl::extension::bmp:
		return LoadBMP(path);
	case bqImageLoaderImpl::extension::jpg:
		return LoadJPG(path);
	case bqImageLoaderImpl::extension::png:
		return LoadPNG(path);
	case bqImageLoaderImpl::extension::tga:
		return LoadTGA(path);
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
	case bqImageLoaderImpl::extension::bmp:
		return LoadBMP(path, buffer, bufferSz);
	case bqImageLoaderImpl::extension::jpg:
		return LoadJPG(path, buffer, bufferSz);
	case bqImageLoaderImpl::extension::png:
		return LoadPNG(path, buffer, bufferSz);
	case bqImageLoaderImpl::extension::tga:
		return LoadTGA(path, buffer, bufferSz);
	}
	return nullptr;
}
