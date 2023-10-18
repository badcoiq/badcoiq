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
#ifndef _EX_g_mario_H_
#define _EX_g_mario_H_

class DemoExample;
class DemoApp;
class ExampleGameMario : public DemoExample
{
	bqCamera* m_camera = 0;

	enum
	{
		gameState_black1, // чёрный фон и текст с номером мира и т.д.
		gameState_black2, // просто чёрный фон
		gameState_game,
	};
	uint32_t m_state = gameState_black1;

	bqTexture* m_texture = 0;

	bqColor m_palette[10] =
	{
		bq::ColorWhite,
		bq::ColorBlack,

		// ground
		0xFF994E00, // brown
		0xFFFFCCC5, // like pink

		bq::ColorTransparent,

		0xFFEA9E22, // box bright
		0xFF561D00, // box dark

		0xFF64B0FF, // cloud blue
		0xFF0D9300, // hill green
		0xFF88D800, // pipe green
	};

	char m_colorMap[127];
	void _initColorMap()
	{
		m_colorMap[' '] = 4; // transparent
		m_colorMap['#'] = 0; // white
		m_colorMap['`'] = 1; // black
		m_colorMap['%'] = 2; // brown
		m_colorMap['^'] = 3; // like pink
		m_colorMap['@'] = 5; // box bright
		m_colorMap['*'] = 6; // box dark
		m_colorMap['['] = 7; // cloud blue
		m_colorMap['+'] = 8; // hill green
		m_colorMap['X'] = 9; // pipe green
	}

	void _imageFill(bqImage* img, bqStringA* str, uint32_t w, uint32_t h, uint32_t whereX, uint32_t whereY);

public:
	ExampleGameMario(DemoApp*);
	virtual ~ExampleGameMario();
	BQ_PLACEMENT_ALLOCATOR(ExampleGameMario);

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual void OnDraw() override;
};

#endif
