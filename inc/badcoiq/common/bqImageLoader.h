﻿/*
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
#ifndef __BQ_IMAGE_LOADER_H__
#define __BQ_IMAGE_LOADER_H__
#ifdef BQ_WITH_IMAGE
#include "bqImage.h"

// Класс для загрузки изображения.
// Библиотеки, загружающие картинки, должны реализовать производный класс.
class bqImageLoader
{
public:
	bqImageLoader() {}
	virtual ~bqImageLoader() {}

	// Получить количество поддерживаемых форматов
	virtual uint32_t GetSupportedFilesCount() = 0;

	// Получить поддерживаемого расширение файла в виде строки
	// Надо указать индекс в соответствии с GetSupportedFilesCount()
	virtual bqString GetSupportedFileExtension(uint32_t) = 0;

	// Получить название поддерживаемого формата
	// Надо указать индекс в соответствии с GetSupportedFilesCount()
	virtual bqString GetSupportedFileName(uint32_t) = 0;

	// Загрузить из файла
	virtual bqImage* Load(const char* path) = 0;

	// Загрузить из памяти
	virtual bqImage* Load(const char* path, uint8_t* buffer, uint32_t bufferSz) = 0;

	virtual bool IsSupportSaveFormat(bqImage::SaveFileFormat) = 0;
	virtual bool Save(bqImage* image, bqImage::SaveFileFormat format, const char* path) = 0;
};

#endif
#endif

