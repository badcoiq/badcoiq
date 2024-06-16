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
#pragma once
#ifndef __BQ_IMGLDRIMPL_H__
#define __BQ_IMGLDRIMPL_H__

#include "badcoiq.h"

#ifdef BQ_WITH_IMAGE
#include "badcoiq/common/bqImage.h"
#include "badcoiq/common/bqImageLoader.h"

class bqImageLoaderImpl : public bqImageLoader
{
	enum class extension { _bad, 
#ifdef BQ_WITH_IMAGE_BMP
		bmp,
#endif
#ifdef BQ_WITH_IMAGE_PNG
		png,
#endif
#ifdef BQ_WITH_IMAGE_JPG
		jpg,
#endif
#ifdef BQ_WITH_IMAGE_TGA
		tga 
#endif
	};
	extension _GetExtension(const char* path);

#ifdef BQ_WITH_IMAGE_BMP
	bqImage* LoadBMP(const char* path);
	bqImage* LoadBMP(const char* path, uint8_t* buffer, uint32_t bufferSz);
	bool SaveBMP(bqImage* image, bqImage::SaveFileFormat format, const char* path);
	bool _save_bmp24(bqImage* image, const char* path);
#endif
#ifdef BQ_WITH_IMAGE_PNG
	bqImage* LoadPNG(const char* path);
	bqImage* LoadPNG(const char* path, uint8_t* buffer, uint32_t bufferSz);
#endif
#ifdef BQ_WITH_IMAGE_JPG
	bqImage* LoadJPG(const char* path);
	bqImage* LoadJPG(const char* path, uint8_t* buffer, uint32_t bufferSz);
#endif
#ifdef BQ_WITH_IMAGE_TGA
	bqImage* LoadTGA(const char* path);
	bqImage* LoadTGA(const char* path, uint8_t* buffer, uint32_t bufferSz);
#endif
	bool SaveDDS(bqImage* image, bqImage::SaveFileFormat format, const char* path);
	bool _save_ddsRGBA8(bqImage* image, const char* path);
public:
	bqImageLoaderImpl();
	virtual ~bqImageLoaderImpl();

	virtual uint32_t GetSupportedFilesCount() override;
	virtual bqString GetSupportedFileExtension(uint32_t) override;
	virtual bqString GetSupportedFileName(uint32_t) override;

	virtual bqImage* Load(const char* path) override;
	virtual bqImage* Load(const char* path, uint8_t* buffer, uint32_t bufferSz) override;

	virtual bool IsSupportSaveFormat(bqImage::SaveFileFormat) override;
	virtual bool Save(bqImage* image, bqImage::SaveFileFormat format, const char* path) override;
};

#endif
#endif
