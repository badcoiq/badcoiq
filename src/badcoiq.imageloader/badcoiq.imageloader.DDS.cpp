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

//#ifdef BQ_WITH_IMAGE_BMP

#include "badcoiq/common/bqFile.h"

struct dds_pixelformat
{
	unsigned int size;
	unsigned int flags;
	unsigned int fourCC;
	unsigned int RGBBitCount;
	unsigned int RBitMask;
	unsigned int GBitMask;
	unsigned int BBitMask;
	unsigned int ABitMask;
};

struct dds_header
{
	unsigned int size;
	unsigned int flags;
	unsigned int height;
	unsigned int width;
	unsigned int pitchOrLinearSize;
	unsigned int depth;
	unsigned int mipMapCount;
	unsigned int reserved1[11];
	struct dds_pixelformat ddspf;
	unsigned int caps;
	unsigned int caps2;
	unsigned int caps3;
	unsigned int caps4;
	unsigned int reserved2;
};


bool bqImageLoaderImpl::SaveDDS(bqImage* image, bqImage::SaveFileFormat format, const char* path)
{
	BQ_ASSERT_ST(image);
	BQ_ASSERT_ST(path);

	switch (format)
	{
	case bqImage::SaveFileFormat::ddsRGBA8:
		return _save_ddsRGBA8(image, path);
		break;
	case bqImage::SaveFileFormat::null:
		break;
	default:
		break;
	}

	return false;
}

bool bqImageLoaderImpl::_save_ddsRGBA8(bqImage* image, const char* path)
{
	BQ_ASSERT_ST(image);
	BQ_ASSERT_ST(path);
	bqFile file;
	if (file.Open(bqFile::_open::Create, path))
	{
		dds_header head;
		memset(&head, 0, sizeof(struct dds_header));
		head.size = sizeof(struct dds_header);
		head.flags = 0x00000002 | 0x00000004;
		head.width = image->m_info.m_width;
		head.height = image->m_info.m_height;
		head.pitchOrLinearSize = image->m_dataSize;
		head.mipMapCount = 1;
		head.ddspf.size = sizeof(struct dds_pixelformat);
		head.ddspf.flags = 0x00000041; /*DDS_RGBA*/
		head.ddspf.RGBBitCount = 32;
		head.ddspf.RBitMask = 0x000000ff;
		head.ddspf.GBitMask = 0x0000ff00;
		head.ddspf.BBitMask = 0x00ff0000;
		head.ddspf.ABitMask = 0xff000000;

		file.WriteUint32(0x20534444);
		file.Write(&head, sizeof(dds_header));
		file.Write(image->m_data, image->m_dataSize);

		return true;
	}

	return false;
}

//#endif
