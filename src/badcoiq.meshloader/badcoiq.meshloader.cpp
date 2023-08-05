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

#include "badcoiq.meshloader.h"

extern "C"
{
	bqMeshLoader* BQ_CDECL bqMeshLoaderDefault_create()
	{
		bqMeshLoaderImpl* ml = bqCreate<bqMeshLoaderImpl>();
		return ml;
	}
}

bqMeshLoaderImpl::bqMeshLoaderImpl() {}
bqMeshLoaderImpl::~bqMeshLoaderImpl() {}

uint32_t bqMeshLoaderImpl::GetSupportedFilesCount()
{
	return 2;
}

bqString bqMeshLoaderImpl::GetSupportedFileExtension(uint32_t i)
{
	switch (i)
	{
	case 0:
		return "obj";
	case 1:
		return "smd";
	}
	return "-";
}

bqString bqMeshLoaderImpl::GetSupportedFileName(uint32_t i)
{
	switch (i)
	{
	case 0:
		return "Wavefront OBJ";
	case 1:
		return "Valve Studiomdl Data";
	}
	return "-";
}

bqMeshLoaderImpl::extension bqMeshLoaderImpl::_GetExtension(const char* path)
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
			if (strcmp(".obj", &path[last_dot]) == 0)
				return extension::obj;
			if (strcmp(".smd", &path[last_dot]) == 0)
				return extension::smd;
		}
	}
	return extension::_bad;
}

void bqMeshLoaderImpl::Load(const char* path, bqMeshLoaderCallback* cb)
{
	auto e = _GetExtension(path);
	switch (e)
	{
	case bqMeshLoaderImpl::extension::_bad:
		break;
	case bqMeshLoaderImpl::extension::obj:
		LoadOBJ(path, cb);
		break;
	case bqMeshLoaderImpl::extension::smd:
	//	LoadSMD(path, cb);
		break;
	}
}

void bqMeshLoaderImpl::Load(const char* path, bqMeshLoaderCallback* cb, uint8_t* buffer, uint32_t bufferSz)
{
	auto e = _GetExtension(path);
	switch (e)
	{
	case bqMeshLoaderImpl::extension::_bad:
		break;
	case bqMeshLoaderImpl::extension::obj:
		LoadOBJ(path, cb, buffer, bufferSz);
		break;
	case bqMeshLoaderImpl::extension::smd:
		//LoadSMD(path, cb, buffer, bufferSz);
		break;
	}
}

