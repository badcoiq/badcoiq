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
#pragma once
#ifndef _BQVGTARGET_H_
#define _BQVGTARGET_H_

class bqVectorGraphicsTarget
{
	bqImage* m_img = 0;
	enum
	{
		mask_hor = 0x1,
		mask_left = 0x2,
		mask_right = 0x4,
		mask_aa = 0x8,
		mask_aa_done = 0x10,
	};
	uint8_t* m_masks = 0;
	float32_t* m_aa = 0;
	uint32_t m_numPixels = 0;
	uint32_t* m_starts = 0;
	uint32_t m_startsSz = 0;
	uint32_t m_targetWidth = 0;

	bqColor m_color;
	uint8_t m_color8[4] = { 0,0,0,255 };

	void _onClear();
public:
	bqVectorGraphicsTarget(uint32_t w, uint32_t h);
	~bqVectorGraphicsTarget();

	bqImage* GetImage() { return m_img; }

	void Clear(const bqColor&);
	void SetColor(const bqColor&);
	void Draw(bqVectorGraphicsShape*);
	void DrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t thinkness);
};

#endif
