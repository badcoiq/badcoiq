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
#ifndef __BQ_SCENESprt_H__
#define __BQ_SCENESprt_H__

#include "badcoiq/common/bqColor.h"
#include "badcoiq/containers/bqArray.h"
#include "badcoiq/scene/bqSceneObject.h"

class bqSpriteState;

// Просто прямоугольник с текстурой
// Можно вращать используя матрицу
// Это не GUI элемент
class bqSprite : public bqSceneObject
{
	bqTexture* m_texture = 0;
	bqVec4f m_rect;
	bqArray<bqSpriteState*> m_states;
	bqSpriteState* m_activeState = 0;
	bqColor m_color = bq::ColorWhite;
public:
	bqSprite();
	virtual ~bqSprite();
	BQ_PLACEMENT_ALLOCATOR(bqSprite);

	bqColor& GetColor() { return m_color; }

	bqTexture* GetTexture() { return m_texture; }
	void SetTexture(bqTexture* t) { m_texture = t; }

	void SetSize(float x, float y);

	// Тип прозрачности
	enum class TransparentType
	{
		Discard,  // текстурка не рисуется. порог устанавливается в m_alphaDiscard
		Blending  // смешивание.
	}
	m_transparentType = TransparentType::Discard;

	float m_alphaDiscard = 0.5f;

	bqVec4f& GetRect() { return m_rect; }
	bqSpriteState* GetActiveState() { return m_activeState; }
	void SetActiveState(bqSpriteState* s) { m_activeState = s; }
	bqSpriteState* CreateNewState();

	void AnimationUpdate(float dt);
};

// Кадры находятся здесь.
class bqSpriteState
{
	friend class bqSprite;
	bqSprite* m_sprite = 0;
public:
	bqSpriteState() {}
	~bqSpriteState() {}
	BQ_PLACEMENT_ALLOCATOR(bqSpriteState);

	bqString m_name;

	bqArray<bqVec4f> m_frames; // UVs
	uint32_t m_frameCurrent = 0;
	uint32_t m_frameNum = 0;

	bool m_invertX = false;
	bool m_invertY = false;

	bool m_isAnimation = false;
	float m_fps = 1.f;
	float m_fpsTime = 1.f;

	bool m_play = true;
	bool m_loop = true;
	float m_updateTimer = 0.f;

	void SetFPS(float newFPS) 
	{
		if (newFPS < 1.f)
			newFPS = 1.f;
		m_fps = newFPS;
		m_fpsTime = 1.f / newFPS;
	}

	void AddFrame(const bqVec4f& rect);
	void AnimationUpdate(float dt);
	void CreateAnimation(uint32_t numFrames, const bqVec2f& frameSize, const bqVec2f& startPosition);
};

#endif

