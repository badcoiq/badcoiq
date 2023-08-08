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
#ifndef __BQ_FRAMEWORKIMPL_H__
#define __BQ_FRAMEWORKIMPL_H__

#include "badcoiq/input/bqInput.h"
#include <vector>

class bqFrameworkImpl
{
public:
	bqFrameworkImpl() 
	{
		for (uint32_t i = 0; i < (uint32_t)bqMatrixType::_count; ++i)
		{
			m_matrixPtrs[i] = 0;
		}
	}
	~bqFrameworkImpl() {}
	BQ_PLACEMENT_ALLOCATOR(bqFrameworkImpl);

	float m_deltaTime = 0.f;
	bqFrameworkCallback* m_frameworkCallback = 0;

	bqInputData m_input;

	bqString m_appPath;

	std::vector<bqGS*> m_gss;
	std::vector<bqImageLoader*> m_imageLoaders;
	std::vector<bqMeshLoader*> m_meshLoaders;

	bqStringA m_fileExistString;
	bqStringA m_fileSizeString;

	bqMat4* m_matrixPtrs[(uint32_t)bqMatrixType::_count];

	// Архивы, сжатие
	bool _compress_fastlz(bqCompressionInfo* info);
	bool _decompress_fastlz(bqCompressionInfo* info);
	void _onDestroy_archive();
	std::vector<bqArchiveZipFile*> m_zipFiles;

	void OnDestroy();
};


#endif
