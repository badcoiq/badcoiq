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
#ifdef BQ_WITH_IMAGE_JPG

#include "badcoiq/common/bqFileBuffer.h"

#include "jpeglib.h"
#include <setjmp.h>

struct my_error_mgr {
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};
typedef struct my_error_mgr* my_error_ptr;

void my_error_exit(j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr)cinfo->err;
	(*cinfo->err->output_message) (cinfo);
	longjmp(myerr->setjmp_buffer, 1);
}

class JPG_Loader {
public:
	JPG_Loader() {}
	~JPG_Loader() {
		jpeg_destroy_decompress(&cinfo);
	}

	bqImage* Load(const char* p, uint8_t* buffer, uint32_t bufferSz)
	{
		bqFileBuffer file(buffer, bufferSz);

		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;
		if (setjmp(jerr.setjmp_buffer)) {
			return 0;
		}
		jpeg_create_decompress(&cinfo);
		jpeg_mem_src(&cinfo, buffer, bufferSz);
		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);


		bqImage* img = bqCreate<bqImage>();

		img->m_info.m_width = cinfo.image_width;
		img->m_info.m_height = cinfo.image_height;
		img->m_info.m_bits = 24;
		img->m_info.m_format = bqImageFormat::r8g8b8;
		img->m_info.m_pitch = cinfo.output_width * cinfo.output_components;
		img->m_dataSize = img->m_info.m_pitch * img->m_info.m_height;
		img->m_data = (uint8_t*)bqMemory::malloc(img->m_dataSize);

		uint8_t* imageDataPtr = img->m_data;

		JSAMPARRAY jbuffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr)&cinfo, JPOOL_IMAGE, img->m_info.m_pitch, 1);

		while (cinfo.output_scanline < cinfo.output_height) {
			jpeg_read_scanlines(&cinfo, jbuffer, 1);
			memcpy(&imageDataPtr[0], jbuffer[0], img->m_info.m_pitch);
			imageDataPtr += img->m_info.m_pitch;
		}
		jpeg_finish_decompress(&cinfo);

		//if (bqFramework::GetImageLoaderConvertToRGBA8())
			img->ConvertTo(bqImageFormat::r8g8b8a8);;

		return img;
	}

	jpeg_decompress_struct cinfo;
	my_error_mgr jerr;
};

bqImage* bqImageLoaderImpl::LoadJPG(const char* path)
{
	BQ_ASSERT_ST(path);

	bqImage* img = 0;
	uint32_t file_size = 0;
	uint8_t* ptr = bqFramework::SummonFileBuffer(path, &file_size, false);
	if (ptr)
	{
		img = LoadJPG(path, ptr, (uint32_t)file_size);
		bqDestroy(ptr);
	}
	return img;
}

bqImage* bqImageLoaderImpl::LoadJPG(const char* path, uint8_t* buffer, uint32_t bufferSz)
{
	BQ_ASSERT_ST(path);
	BQ_ASSERT_ST(buffer);
	BQ_ASSERT_ST(bufferSz);

	JPG_Loader loader;
	return loader.Load(path, buffer, bufferSz);
}

#endif
