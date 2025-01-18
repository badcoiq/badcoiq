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

#ifdef BQ_WITH_IMAGE_BMP

#include "badcoiq/common/bqFileBuffer.h"
#include "badcoiq/common/bqFile.h"

bqImage* bqImageLoaderImpl_LoadBMP(bqFileBuffer*);

bqImage* bqImageLoaderImpl::LoadBMP(const char* path)
{
	BQ_ASSERT_ST(path);

	bqImage* img = 0;
	uint32_t file_size = 0;
	uint8_t* ptr = bqFramework::CreateFileBuffer(path, &file_size, false);
	if (ptr)
	{
		img = LoadBMP(path, ptr, (uint32_t)file_size);
		bqDestroy(ptr);
	}
	return img;
}

// 32bitRGBA
// 32bitRGB
// 24bit
// 16bit565RGB
// 16bit5551RGBA
// 16bit5551RGB
bqImage* bqImageLoaderImpl::LoadBMP(const char* path, uint8_t* buffer, uint32_t bufferSz)
{
	BQ_ASSERT_ST(path);  // Нафига вообще нужен path? 
	BQ_ASSERT_ST(buffer);
	BQ_ASSERT_ST(bufferSz);

	bqFileBuffer file(buffer, bufferSz);

	return bqImageLoaderImpl_LoadBMP(&file);
}

bool bqImageLoaderImpl::SaveBMP(bqImage* image, bqImage::SaveFileFormat format, const char* path)
{
	BQ_ASSERT_ST(image);
	BQ_ASSERT_ST(path);

	switch (format)
	{
	case bqImage::SaveFileFormat::bmp24:
		return _save_bmp24(image, path);
	}

	return false;
}

bool bqImageLoaderImpl::_save_bmp24(bqImage* image, const char* path)
{
	bqFile file;
	if (file.Open(bqFile::_open::Create, path))
	{
		uint32_t offset = 54;
		
		uint32_t rowSz = image->m_info.m_width * 3;
		
		uint32_t padding = rowSz;
		
		if (padding % 4)
		{
			++padding;
			if (padding % 4)
			{
				++padding;
				if (padding % 4)
				{
					++padding;
					if (padding % 4)
					{
						//printf(" (╯°□°)╯︵ ┻━┻ \n");
					}
				}
			}
		}

		padding -= rowSz;


		uint32_t imageDataSz = (rowSz + padding) * image->m_info.m_height;
		uint32_t fileSz = offset + imageDataSz;

		// Bitmap file header
		file.WriteString("BM", 2);
		file.WriteUint32(fileSz);
		file.WriteUint16(0); // reserved
		file.WriteUint16(0); // reserved
		file.WriteUint32(offset);

		// DIB header 
		file.WriteUint32(40); // header size
		file.WriteUint32(image->m_info.m_width);
		file.WriteUint32(image->m_info.m_height);
		file.WriteUint16(1); // color planes
		file.WriteUint16(24); // bits
		file.WriteUint32(0); //  compression method
		file.WriteUint32(imageDataSz);
		file.WriteInt32(0); // X pixels per meter
		file.WriteInt32(0); // Y pixels per meter
		file.WriteUint32(0); // number of colors
		file.WriteUint32(0); // number of important colors used

		uint8_t* dataLn = image->m_data + (image->m_info.m_pitch * image->m_info.m_height) - image->m_info.m_pitch;
		uint8_t* data = dataLn;
		
		for (uint32_t i = 0; i < image->m_info.m_height; ++i)
		{
			for (uint32_t o = 0; o < image->m_info.m_width; ++o)
			{
				file.WriteUint8(data[2]);
				file.WriteUint8(data[1]);
				file.WriteUint8(data[0]);

				data += 3;

				if (image->m_info.m_format == bqImageFormat::r8g8b8a8)
					++data;
			}

			for (uint32_t o = 0; o < padding; ++o)
			{
				file.WriteUint8(0);
			}

			dataLn -= image->m_info.m_pitch;
			data = dataLn;
		}

		return true;
	}

	return false;
}

#endif
