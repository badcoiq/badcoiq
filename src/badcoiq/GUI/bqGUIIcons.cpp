﻿/*
BSD 2-Clause License

Copyright (c) 2025, badcoiq
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

#ifdef BQ_WITH_GUI


#include "badcoiq/GUI/bqGUI.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/input/bqInputEx.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqGUIIcons::bqGUIIcons(bqGUIIconTexture* t)
	:
	m_texture(t)
{
}

bqGUIIcons::~bqGUIIcons()
{
}

int bqGUIIcons::Add(int left, int top, int sizeX, int sizeY)
{
	BQ_ASSERT_ST(m_texture);

	int id = (int)m_nodes.size();
	
	_node n;
	n.lt.x = left;
	n.lt.y = top;
	n.rb.x = left + sizeX;
	n.rb.y = top + sizeY;
	n.sz.x = sizeX;
	n.sz.y = sizeY;

	bqVec2i tsz;
	m_texture->GetSize(&tsz);

	float32_t mx = 1.f / (float32_t)tsz.x;
	float32_t my = 1.f / (float32_t)tsz.y;

	n.uv.x = n.lt.x * mx;
	n.uv.y = n.lt.y * my;
	n.uv.z = n.rb.x * mx;
	n.uv.w = n.rb.y * my;

	m_nodes.push_back(n);

	return id;
}

#endif
