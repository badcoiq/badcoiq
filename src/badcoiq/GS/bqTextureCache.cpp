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

#include "badcoiq/gs/bqTexture.h"
#include "badcoiq/geometry/bqMeshLoader.h"

#ifdef BQ_WITH_GS

#include "badcoiq/gs/bqGS.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqTextureCache::bqTextureCache(bqGS* gs)
{
	m_gs = gs;
//	m_data.reserve(100);
}

bqTextureCache::~bqTextureCache()
{
	Clear();
}

void bqTextureCache::Free(bqTexture* r)
{
	BQ_SAFEDESTROY(r);
}

bqTexture* bqTextureCache::Load(const char* p)
{
	bqTexture* t = 0;
	bqImage* img = bqFramework::SummonImage(p);
	if (img)
	{
		t = m_gs->SummonTexture(img, m_textureInfo);
		delete img;
	}
	return t;
}

bqGPUMeshCache::bqGPUMeshCache(bqGS* gs):m_gs(gs) {}
bqGPUMeshCache::~bqGPUMeshCache() {}
void bqGPUMeshCache::Free(bqGPUMesh* r) 
{
	BQ_SAFEDESTROY(r);
}
bqGPUMesh* bqGPUMeshCache::Load(const char* path) 
{
	bqGPUMesh* gm = 0;
	bqMesh* m = bqFramework::SummonMesh(path);
	if (m)
	{
		gm = m_gs->SummonMesh(m);
		delete m;
	}
	return gm;
}

#endif
