/*
BSD 2-Clause License

Copyright (c) 2023-2025, badcoiq
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
#ifndef __BQ_GUIICONS_H__
/// \cond
#define __BQ_GUIICONS_H__
/// \endcond

#ifdef BQ_WITH_GUI

#include "badcoiq/math/bqMath.h"

/// Текстура/картинка для bqGUIIcons
/// bqGUIIcons может быть использована не только в обычном GUI
/// работающем на текущем GPU драйвере, но она так-же может
/// быть использована в GUI реализованном на других API.
/// Например, на GUI работающим на GDI. Для этого оно и делалось.
class bqGUIIconTexture
{
public:
	bqGUIIconTexture() {}
	virtual ~bqGUIIconTexture() {}
	BQ_PLACEMENT_ALLOCATOR(bqGUIIconTexture);

	virtual void GetSize(bqVec2i*) = 0;
};



/// Заранее приготовленные области(левый-верхний угол и правый-нижний)
class bqGUIIcons 
{
	bqGUIIconTexture* m_texture = 0;

	struct _node
	{
		bqPoint lt, rb, sz;
		bqVec4f uv;
	};

	bqArray<_node> m_nodes;
public:
	bqGUIIcons(bqGUIIconTexture*);
	~bqGUIIcons();

	int Add(int left, int top, int sizeX, int sizeY);
	bqGUIIconTexture* GetTexture() { return m_texture; }
};

#endif
#endif
