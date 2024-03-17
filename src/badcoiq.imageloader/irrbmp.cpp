/*
  Copyright (C) 2002-2012 Nikolaus Gebhardt
  Copyright (c) 2024, badcoiq

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
	 claim that you wrote the original software. If you use this software
	 in a product, an acknowledgment in the product documentation would be
	 appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
	 misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Please note that the Irrlicht Engine is based in part on the work of the
  Independent JPEG Group, the zlib, libPng and aesGladman. This means that if you use
  the Irrlicht Engine in your product, you must acknowledge somewhere in your
  documentation that you've used the IJPG code. It would also be nice to mention
  that you use the Irrlicht Engine, the zlib, libPng and aesGladman. See the
  corresponding license files for further informations. It is also possible to disable
  usage of those additional libraries by defines in the IrrCompileConfig.h header and
  recompiling the engine.

*/


#include "badcoiq.h"
#include "badcoiq.imageloader.h"
#include "badcoiq/common/bqFileBuffer.h"


void BMP_decompress8BitRLE(uint8_t*& bmpData, int32_t size, int32_t width, int32_t height, int32_t pitch)
{
	uint8_t* p = bmpData;
	uint8_t* newBmp = new uint8_t[(width + pitch) * height];
	uint8_t* d = newBmp;
	uint8_t* destEnd = newBmp + (width + pitch) * height;
	int32_t line = 0;

	while (bmpData - p < size && d < destEnd)
	{
		if (*p == 0)
		{
			++p;

			switch (*p)
			{
			case 0: // end of line
				++p;
				++line;
				d = newBmp + (line * (width + pitch));
				break;
			case 1: // end of bmp
				delete[] bmpData;
				bmpData = newBmp;
				return;
			case 2:
				++p; d += (uint8_t)*p;  // delta
				++p; d += ((uint8_t)*p) * (width + pitch);
				++p;
				break;
			default:
			{
				// absolute mode
				int32_t count = (uint8_t)*p; ++p;
				int32_t readAdditional = ((2 - (count % 2)) % 2);
				int32_t i;

				for (i = 0; i < count; ++i)
				{
					*d = *p;
					++p;
					++d;
				}

				for (i = 0; i < readAdditional; ++i)
					++p;
			}
			}
		}
		else
		{
			int32_t count = (uint8_t)*p; ++p;
			uint8_t color = *p; ++p;
			for (int32_t i = 0; i < count; ++i)
			{
				*d = color;
				++d;
			}
		}
	}

	delete[] bmpData;
	bmpData = newBmp;
}


void BMP_decompress4BitRLE(uint8_t*& bmpData, int32_t size, int32_t width, int32_t height, int32_t pitch)
{
	int32_t lineWidth = (width + 1) / 2 + pitch;
	uint8_t* p = bmpData;
	uint8_t* newBmp = new uint8_t[lineWidth * height];
	uint8_t* d = newBmp;
	uint8_t* destEnd = newBmp + lineWidth * height;
	int32_t line = 0;
	int32_t shift = 4;

	while (bmpData - p < size && d < destEnd)
	{
		if (*p == 0)
		{
			++p;

			switch (*p)
			{
			case 0: // end of line
				++p;
				++line;
				d = newBmp + (line * lineWidth);
				shift = 4;
				break;
			case 1: // end of bmp
				delete[] bmpData;
				bmpData = newBmp;
				return;
			case 2:
			{
				++p;
				int32_t x = (uint8_t)*p; ++p;
				int32_t y = (uint8_t)*p; ++p;
				d += x / 2 + y * lineWidth;
				shift = x % 2 == 0 ? 4 : 0;
			}
			break;
			default:
			{
				// absolute mode
				int32_t count = (uint8_t)*p; ++p;
				int32_t readAdditional = ((2 - ((count) % 2)) % 2);
				int32_t readShift = 4;
				int32_t i;

				for (i = 0; i < count; ++i)
				{
					int32_t color = (((uint8_t)*p) >> readShift) & 0x0f;
					readShift -= 4;
					if (readShift < 0)
					{
						++* p;
						readShift = 4;
					}

					uint8_t mask = 0x0f << shift;
					*d = (*d & (~mask)) | ((color << shift) & mask);

					shift -= 4;
					if (shift < 0)
					{
						shift = 4;
						++d;
					}

				}

				for (i = 0; i < readAdditional; ++i)
					++p;
			}
			}
		}
		else
		{
			int32_t count = (uint8_t)*p; ++p;
			int32_t color1 = (uint8_t)*p; color1 = color1 & 0x0f;
			int32_t color2 = (uint8_t)*p; color2 = (color2 >> 4) & 0x0f;
			++p;

			for (int32_t i = 0; i < count; ++i)
			{
				uint8_t mask = 0x0f << shift;
				uint8_t toSet = (shift == 0 ? color1 : color2) << shift;
				*d = (*d & (~mask)) | (toSet & mask);

				shift -= 4;
				if (shift < 0)
				{
					shift = 4;
					++d;
				}
			}
		}
	}

	delete[] bmpData;
	bmpData = newBmp;
}

#pragma pack(push,2)
struct SBMPHeader
{
	uint16_t	Id;					//	BM - Windows 3.1x, 95, NT, 98, 2000, ME, XP
										//	BA - OS/2 Bitmap Array
										//	CI - OS/2 Color Icon
										//	CP - OS/2 Color Pointer
										//	IC - OS/2 Icon
										//	PT - OS/2 Pointer
	uint32_t	FileSize;
	uint32_t	Reserved;
	uint32_t	BitmapDataOffset;
	uint32_t	BitmapHeaderSize;	// should be 28h for windows bitmaps or
										// 0Ch for OS/2 1.x or F0h for OS/2 2.x
	uint32_t Width;
	uint32_t Height;
	uint16_t Planes;
	uint16_t BPP;					// 1: Monochrome bitmap
										// 4: 16 color bitmap
										// 8: 256 color bitmap
										// 16: 16bit (high color) bitmap
										// 24: 24bit (true color) bitmap
										// 32: 32bit (true color) bitmap

	uint32_t  Compression;			// 0: none (Also identified by BI_RGB)
										// 1: RLE 8-bit / pixel (Also identified by BI_RLE4)
										// 2: RLE 4-bit / pixel (Also identified by BI_RLE8)
										// 3: Bitfields  (Also identified by BI_BITFIELDS)

	uint32_t  BitmapDataSize;		// Size of the bitmap data in bytes. This number must be rounded to the next 4 byte boundary.
	uint32_t  PixelPerMeterX;
	uint32_t  PixelPerMeterY;
	uint32_t  Colors;
	uint32_t  ImportantColors;

	uint32_t RedMask;		//	для определения позиции цветов
	uint32_t GreenMask;	//	в форматах типа x1r5g5b5
	uint32_t BlueMask;
	uint32_t AlphaMask;
};
#pragma pack(pop)

bqImage* bqImageLoaderImpl_LoadBMP(bqFileBuffer* file)
{
	SBMPHeader header;

	file->Read(&header, 54);

	if (header.BitmapHeaderSize > 40 || header.Compression == 3)
	{
		file->Seek(0, SEEK_SET);
		file->Read(&header, sizeof(header));
		file->Seek(54, SEEK_SET);
	}

	int32_t pitch = 0;

	//! return if the header is false

	if (header.Id != 0x4d42)
		return 0;

	//if (header.Compression > 2) // we'll only handle RLE-Compression
	//{
	//	slLog::PrintError("Compression mode not supported.");
	//	return 0;
	//}

	// adjust bitmap data size to dword boundary
	header.BitmapDataSize += (4 - (header.BitmapDataSize % 4)) % 4;

	// read palette

	size_t pos = file->Tell();
	int32_t paletteSize = (header.BitmapDataOffset - pos) / 4;

	int32_t* paletteData = 0;
	if (paletteSize)
	{
		paletteData = new int32_t[paletteSize];
		file->Read(paletteData, paletteSize * sizeof(int32_t));
	}

	// read image data

	if (!header.BitmapDataSize)
	{
		// okay, lets guess the size
		// some tools simply don't set it
		header.BitmapDataSize = static_cast<uint32_t>(file->Size()) - header.BitmapDataOffset;
	}

	file->Seek(header.BitmapDataOffset, SEEK_SET);

	float t = (header.Width) * (header.BPP / 8.0f);
	int32_t widthInBytes = (int32_t)t;
	t -= widthInBytes;
	if (t != 0.0f)
		++widthInBytes;

	int32_t lineData = widthInBytes + ((4 - (widthInBytes % 4))) % 4;
	pitch = lineData - widthInBytes;

	uint8_t* bmpData = new uint8_t[header.BitmapDataSize];
	file->Read(bmpData, header.BitmapDataSize);

	// decompress data if needed
	switch (header.Compression)
	{
	case 1: // 8 bit rle
		BMP_decompress8BitRLE(bmpData, header.BitmapDataSize, header.Width, header.Height, pitch);
		break;
	case 2: // 4 bit rle
		BMP_decompress4BitRLE(bmpData, header.BitmapDataSize, header.Width, header.Height, pitch);
		break;
	}

	// create surface

	bqImageInfo imageInfo;
	imageInfo.m_width = static_cast<uint32_t>(header.Width);
	imageInfo.m_height = static_cast<uint32_t>(header.Height);
	imageInfo.m_bits = 32;
	imageInfo.m_pitch = imageInfo.m_width * 4; // convert all into RGBA8

	bqImage* image = bqCreate<bqImage>();
	image->m_dataSize = imageInfo.m_pitch * imageInfo.m_height;
	image->m_data = (uint8_t*)bqMemory::malloc(image->m_dataSize);
	image->m_info = imageInfo;

	uint8_t* src = bmpData;
	uint8_t* dst = image->m_data;

	uint32_t forBits = 256; // 0000 0001 0000 0000
	forBits |= *src;

	for (size_t i = 0; i < imageInfo.m_height; ++i)
	{
		uint8_t* src2 = src;

		for (size_t o = 0; o < imageInfo.m_width; ++o)
		{
			switch (header.BPP)
			{
			case 1: {
				uint8_t* dst2 = dst + (o * 4);
				int C = (forBits & 128) ? 255 : 0;
				dst2[0] = C;
				dst2[1] = C;
				dst2[2] = C;
				dst2[3] = 255;
				
				forBits = forBits << 1;
				if (forBits >= 65536)
				{
					++src2;
					forBits = 256 | *src2;
				}
			}break;
			case 4:{
				uint8_t * dst2 = dst + (o * 4);

				int ind = (forBits & 240) >> 4;

				dst2[0] = (paletteData[ind] & 0xFF0000) >> 16;
				dst2[1] = (paletteData[ind] & 0xFF00) >> 8;
				dst2[2] = (paletteData[ind] & 0xFF);
				dst2[3] = 255;

				forBits = forBits << 4;
				if (forBits >= 65536)
				{
					++src2;
					forBits = 256 | *src2;
				}
			}break;
			case 8: {
				uint8_t* dst2 = dst + (o * 4);

				int ind = src[o];

				dst2[0] = (paletteData[ind] & 0xFF0000) >> 16;
				dst2[1] = (paletteData[ind] & 0xFF00) >> 8;
				dst2[2] = (paletteData[ind] & 0xFF);
				dst2[3] = 255;

			}break;
			case 16:{
				uint16_t pixel = src[o * 2];
				pixel |= src[(o * 2) + 1] << 8;
				
				if (header.Compression == 0)
				{
					uint8_t* dst2 = dst + (o * 4);
					dst2[0] = ((pixel & 0xFC00) >> 10) * 8;
					dst2[1] = ((pixel & 0x3E0) >> 5) * 8;
					dst2[2] = ((pixel & 0x1F)) * 8;
					dst2[3] = 255;
				}
				else if (header.Compression == 3)
				{
					int shift1 = 0;
					int shift2 = 0;

					switch (header.RedMask){
					case 31744:
						shift1 = 10;
						break;
					case 63488:
						shift1 = 11;
						break;
					}

					switch (header.GreenMask) {
					case 992:
						shift2 = 5;
						break;
					case 2016:
						shift2 = 6;
						break;
					}

					uint8_t* dst2 = dst + (o * 4);
					dst2[0] = ((pixel & header.RedMask) >> shift1) * 8;
					dst2[1] = ((pixel & header.GreenMask) >> shift2) * 8;
					dst2[2] = ((pixel & header.BlueMask)) * 8;
					dst2[3] = 255;
				}
			}break;
			case 24: {
				uint8_t * dst2 = dst + (o * 4);
				int ind = o * 3;
				dst2[0] = src[ind + 2];
				dst2[1] = src[ind + 1];
				dst2[2] = src[ind + 0];
				dst2[3] = 255;
			}break;
			case 32:
				uint8_t * dst2 = dst + (o * 4);
				int ind = o * 4;
				dst2[0] = src[ind + 2];
				dst2[1] = src[ind + 1];
				dst2[2] = src[ind + 0];
				dst2[3] = 255;
				break;
			};
		}

		src += widthInBytes + pitch;
		dst += imageInfo.m_pitch;
	}
	//if (image)
	//	image->unlock();

	// clean up

	delete[] paletteData;
	delete[] bmpData;

	image->FlipVertical();

	return image;
}