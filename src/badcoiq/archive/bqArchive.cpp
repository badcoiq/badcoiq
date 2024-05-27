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
#ifdef BQ_WITH_ARCHIVE

#include "badcoiq/archive/bqArchive.h"

#include "unzip.h"
#include "fastlz.h"

#include <filesystem>

#include "../framework/bqFrameworkImpl.h"

extern bqFrameworkImpl* g_framework;

BQ_LINK_LIBRARY("minizip");
BQ_LINK_LIBRARY("fastlz");
BQ_LINK_LIBRARY("zlib");
BQ_LINK_LIBRARY("bzip2");


void bqFrameworkImpl::_onDestroy_archive()
{
	for (size_t i = 0; i < m_zipFiles.size(); ++i)
	{
		if (m_zipFiles[i]->m_implementation)
		{
			unzClose(m_zipFiles[i]->m_implementation);
		}
		bqDestroy(m_zipFiles[i]);
	}
}

bqArchiveZipFile* bqArchiveSystem::ZipAdd(const char* fn)
{
	BQ_ASSERT_ST(fn);
	bqLog::Print("AddZip: [%s]\n", fn);

	if (!std::filesystem::exists(fn))
	{
		bqLog::PrintWarning("AddZip: Unable to find zip file [%s]\n", fn);
		return 0;
	}
	
	unzFile uf = unzOpen64(fn);
	if (uf == NULL)
	{
		bqLog::PrintWarning("AddZip: unzOpen64 failed [%s]\n", fn);
		return 0;
	}

//	unz_global_info64 gi;
//	unzGetGlobalInfo64(uf, &gi);
	
	if (unzGoToFirstFile(uf) == UNZ_OK)
	{
		char* fileName = (char*)bqMemory::calloc(0xffff);

		while (true)
		{
			unz_file_info64 info;
			unzGetCurrentFileInfo64(uf, &info, fileName, 0xffff, 0, 0, 0, 0);

			if(info.uncompressed_size)
				bqLog::Print("AddZip: + %s\n", fileName);

			if (unzGoToNextFile(uf) != UNZ_OK)
			{
				break;
			}
		}

		bqMemory::free(fileName);
	}

	bqArchiveZipFile* zp = bqCreate<bqArchiveZipFile>();
	zp->m_fileName.assign(fn);
	zp->m_implementation = uf;

	g_framework->m_zipFiles.push_back(zp);

	return zp;
}

void bqArchiveSystem::ZipRemove(bqArchiveZipFile* a)
{
	BQ_ASSERT_ST(a);

	for (size_t i = 0; i < g_framework->m_zipFiles.size(); ++i)
	{
		if (g_framework->m_zipFiles[i] == a)
		{
			g_framework->m_zipFiles.erase(g_framework->m_zipFiles.begin() + i);
			bqLog::Print("Remove Zip: %s\n", a->m_fileName.data());
			return;
		}
	}

}

uint8_t* bqArchiveSystem::ZipUnzip(const char* fileInZip, uint32_t* size, bqArchiveZipFile* a)
{
	BQ_ASSERT_ST(fileInZip);
	BQ_ASSERT_ST(size);
	if (a)
		goto lockate_file;

	for (size_t i = 0, sz = g_framework->m_zipFiles.size(); i < sz; ++i)
	{
		if (unzLocateFile(g_framework->m_zipFiles[i]->m_implementation, fileInZip, 0) == UNZ_OK)
		{
			a = g_framework->m_zipFiles[i];
			goto unzip_file;
		}
	}

	bqLog::PrintWarning("file %s not found in the zipfile\n", fileInZip);
	return 0;

lockate_file:;
	if (unzLocateFile(a->m_implementation, fileInZip, 0) != UNZ_OK)
	{
		bqLog::PrintWarning("file %s not found in the zipfile\n", fileInZip);
		return 0;
	}

unzip_file:;
	char filename_inzip[256];

	unz_file_info64 file_info;
	int err = unzGetCurrentFileInfo64(a->m_implementation, &file_info,
		filename_inzip,
		sizeof(filename_inzip),
		NULL, 0, NULL, 0);

	if (err != UNZ_OK)
	{
		bqLog::PrintWarning("error %d with zipfile in unzGetCurrentFileInfo [%s]\n", err, fileInZip);
		return 0;
	}

	err = unzOpenCurrentFilePassword(a->m_implementation, 0);
	if (err != UNZ_OK)
	{
		bqLog::PrintWarning("error %d with zipfile in unzOpenCurrentFilePassword [%s]\n", err, fileInZip);
		return 0;
	}

	uint8_t* data = (uint8_t*)bqMemory::malloc((size_t)file_info.uncompressed_size);
	*size = (uint32_t)file_info.uncompressed_size;

	err = unzReadCurrentFile(a->m_implementation, data, (unsigned int)file_info.uncompressed_size);
	if (err < 0)
	{
		bqLog::PrintWarning("error %d with zipfile in unzReadCurrentFile [%s]\n", err, fileInZip);
		free(data);
		data = 0;
	}

	unzCloseCurrentFile(a->m_implementation);

	return data;
}

bool bqArchiveSystem::Compress(bqCompressionInfo* info)
{
	BQ_ASSERT_ST(info);
	BQ_ASSERT_ST(info->m_dataUncompressed);
	BQ_ASSERT_ST(info->m_sizeUncompressed);

	info->m_dataCompressed = 0;
	info->m_sizeCompressed = 0;

	switch (info->m_compressorType)
	{
	case bqCompressorType::fastlz:
		return g_framework->_compress_fastlz(info);
	}
	return false;
}

bool bqArchiveSystem::Decompress(bqCompressionInfo* info)
{
	BQ_ASSERT_ST(info);
	BQ_ASSERT_ST(info->m_dataCompressed);
	BQ_ASSERT_ST(info->m_sizeCompressed);
	BQ_ASSERT_ST(info->m_dataUncompressed);
	BQ_ASSERT_ST(info->m_sizeUncompressed);

	switch (info->m_compressorType)
	{
	case bqCompressorType::fastlz:
		return g_framework->_decompress_fastlz(info);
	}
	return false;
}

bool bqArchiveSystem::Decompress(bqCompressionInfo* info, bqArray<uint8_t>& toVector)
{
	BQ_ASSERT_ST(info);

	toVector.clear();
	toVector.reserve(info->m_sizeUncompressed);
	if (!info->m_dataCompressed || !info->m_sizeCompressed)
	{
		if (info->m_dataUncompressed && info->m_sizeUncompressed)
		{
			// НАДО РЕАЛИЗОВАТЬ ЭТО ДЛЯ bqArray !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			std::copy(info->m_dataUncompressed,
				info->m_dataUncompressed + info->m_sizeUncompressed,
				std::back_inserter(toVector));

			return true;
		}
		return false;
	}

	switch (info->m_compressorType)
	{
	case bqCompressorType::fastlz:
	{
		bqCompressionInfo cmpInf = *info;
		cmpInf.m_dataUncompressed = (uint8_t*)bqMemory::malloc(cmpInf.m_sizeUncompressed);
		bool result = g_framework->_decompress_fastlz(&cmpInf);
		if (result)
		{
			// НАДО РЕАЛИЗОВАТЬ ЭТО ДЛЯ bqArray !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			std::copy(
				cmpInf.m_dataUncompressed,
				cmpInf.m_dataUncompressed + cmpInf.m_sizeUncompressed,
				std::back_inserter(toVector));
		}
		bqMemory::free(cmpInf.m_dataUncompressed);
		return result;
	}break;
	}
	return false;
}

bool bqFrameworkImpl::_compress_fastlz(bqCompressionInfo* info)
{
	if (info->m_level != 1 && info->m_level != 2)
		info->m_level = 2;

	if (info->m_sizeUncompressed < 32)
		return false;

	uint8_t* output = (uint8_t*)bqMemory::malloc(info->m_sizeUncompressed + (info->m_sizeUncompressed / 3));

	int compressed_size = fastlz_compress_level(
		(int)info->m_level,
		info->m_dataUncompressed,
		info->m_sizeUncompressed,
		output);

	if ((uint32_t)compressed_size >= info->m_sizeUncompressed)
	{
		bqMemory::free(output);
		return false;
	}

	output = (uint8_t*)bqMemory::realloc(output, compressed_size);

	info->m_dataCompressed = output;
	info->m_sizeCompressed = compressed_size;
	return true;
}

bool bqFrameworkImpl::_decompress_fastlz(bqCompressionInfo* info)
{
	uint32_t decompressed_size = fastlz_decompress(
		info->m_dataCompressed,
		info->m_sizeCompressed,
		info->m_dataUncompressed,
		info->m_sizeUncompressed);
	if (!decompressed_size)
	{
		bqLog::PrintWarning("%s: %s\n", __FUNCTION__, "can't decompress");
		return false;
	}

	if (info->m_sizeUncompressed != decompressed_size)
	{
		info->m_sizeUncompressed = decompressed_size;
		bqLog::PrintWarning("%s: %s\n", __FUNCTION__, "`decompressed size` is not the sme with m_sizeUncompressed");
	}
	return true;
}

void bqArchiveSystem::GetFileList(bqArchiveZipFile* f, bqArray<bqStringA>& a)
{
	BQ_ASSERT_ST(f);

	if (unzGoToFirstFile(f->m_implementation) == UNZ_OK)
	{
		char* fileName = (char*)bqMemory::calloc(0xffff);

		while (true)
		{
			unz_file_info64 info;
			unzGetCurrentFileInfo64(f->m_implementation, &info, fileName, 0xffff, 0, 0, 0, 0);

			if (info.uncompressed_size)
				a.push_back(fileName);

			if (unzGoToNextFile(f->m_implementation) != UNZ_OK)
			{
				break;
			}
		}

		bqMemory::free(fileName);
	}
}

#endif
