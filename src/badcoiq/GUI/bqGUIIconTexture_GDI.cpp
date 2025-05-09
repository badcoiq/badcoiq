/*
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
#ifdef BQ_PLATFORM_WINDOWS


#include "badcoiq/GUI/bqGUI.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/input/bqInputEx.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

#include "bqGUIIconTexture_GDI.h"

bqGUIIconTexture_GDI::bqGUIIconTexture_GDI()
{
}

bqGUIIconTexture_GDI::~bqGUIIconTexture_GDI()
{
	_deleteImage();
}

void bqGUIIconTexture_GDI::_deleteImage()
{
	if (m_image)
	{
		delete m_image;
		m_image = 0;
	}
}


void bqGUIIconTexture_GDI::GetSize(bqVec2i* p)
{
	BQ_ASSERT_ST(p);

	if (p)
		*p = m_size;
}

bool bqGUIIconTexture_GDI::_create(bqImage* img)
{
	BQ_ASSERT_ST(img);

	_deleteImage();

	bool ret = false;
	if (img)
	{
		m_image = new bqImage;
		img->Duplicate(m_image);
		ret = true;
	}

	return ret;
}

#endif
#endif
