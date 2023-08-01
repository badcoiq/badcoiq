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
#ifndef __BQ_IMGLDRIMPL_H__
#define __BQ_IMGLDRIMPL_H__

#include "badcoiq.h"
#include "badcoiq/common/bqImage.h"
#include "badcoiq/common/bqImageLoader.h"

class bqImageLoaderImpl : public bqImageLoader
{
	enum class extension { _bad, bmp, png, jpg, tga };
	extension _GetExtension(const char* path);

	bqImage* LoadBMP(const char* path);
	bqImage* LoadBMP(const char* path, uint8_t* buffer, uint32_t bufferSz);
	bqImage* LoadPNG(const char* path);
	bqImage* LoadPNG(const char* path, uint8_t* buffer, uint32_t bufferSz);
	bqImage* LoadJPG(const char* path);
	bqImage* LoadJPG(const char* path, uint8_t* buffer, uint32_t bufferSz);
	bqImage* LoadTGA(const char* path);
	bqImage* LoadTGA(const char* path, uint8_t* buffer, uint32_t bufferSz);
public:
	bqImageLoaderImpl();
	virtual ~bqImageLoaderImpl();

	virtual uint32_t GetSupportedFilesCount() final;
	virtual bqString GetSupportedFileExtension(uint32_t) final;
	virtual bqString GetSupportedFileName(uint32_t) final;

	virtual bqImage* Load(const char* path) final;
	virtual bqImage* Load(const char* path, uint8_t* buffer, uint32_t bufferSz) final;
};

#endif
