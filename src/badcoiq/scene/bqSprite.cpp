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

#ifdef BQ_WITH_SPRITE

#include "badcoiq/scene/bqSprite.h"
#include "badcoiq/gs/bqTexture.h"

bqSprite::bqSprite()
{
}

bqSprite::bqSprite(bqTexture* t)
{
	SetTexture(t);
}

bqSprite::~bqSprite()
{
	for (size_t i = 0; i < m_states.m_size; ++i)
	{
		delete m_states.m_data[i];
	}
}

void bqSprite::SetSize(float x, float y)
{
	float hx = x * 0.5f;
	float hy = y * 0.5f;

	m_rect.Set(-hx, -hy, hx, hy);

	GetAabb().Add(-hx, -hy, 0.f);
	GetAabb().Add(hx, hy, 0.f);
	GetAabb().Radius();
}

bqSpriteState* bqSprite::CreateNewState()
{
	bqSpriteState* state = new bqSpriteState();
	state->m_sprite = this;
	m_states.push_back(state);
	return state;
}

void bqSprite::AnimationUpdate(float dt)
{
	BQ_ASSERT_ST(m_activeState);
	m_activeState->AnimationUpdate(dt);
}

void bqSpriteState::AddFrame(const bqVec4f& rect)
{
	bqVec2f textureSize((float)m_sprite->GetTexture()->GetInfo().m_imageInfo.m_width,
		(float)m_sprite->GetTexture()->GetInfo().m_imageInfo.m_height);

	float xMulFactor = 1.f / (float)textureSize.x;
	float yMulFactor = 1.f / (float)textureSize.y;

	bqVec4f newFrame;
	newFrame.x = rect.x * xMulFactor;
	newFrame.y = (rect.y * yMulFactor);
	newFrame.z = (rect.z + 1.f) * xMulFactor;
	newFrame.w = (rect.w + 1.f) * yMulFactor;
	this->m_frames.push_back(newFrame);
	m_frameNum = m_frames.size();
}

void bqSpriteState::CreateAnimation(uint32_t numFrames, const bqVec2f& frameSize, const bqVec2f& startPosition)
{
	bqVec2f textureSize((float)m_sprite->GetTexture()->GetInfo().m_imageInfo.m_width,
		(float)m_sprite->GetTexture()->GetInfo().m_imageInfo.m_height);

	bqVec2f sp = startPosition;
	for (uint32_t i = 0; i < numFrames; ++i)
	{
		bqVec4f f;

		f.x = sp.x;
		f.y = sp.y;
		f.z = f.x + frameSize.x - 1.f;
		f.w = f.y + frameSize.y - 1.f;

		if (f.z > textureSize.x)
		{
			sp.x = 0.f;
			sp.y += frameSize.y;

			f.x = sp.x;
			f.y = sp.y;
			f.z = f.x + frameSize.x - 1.f;
			f.w = f.y + frameSize.y - 1.f;
		}

		AddFrame(f);
		m_isAnimation = true;

		sp.x += frameSize.x;
	}
}

void bqSpriteState::AnimationUpdate(float dt)
{
	if (m_isAnimation && m_play)
	{
		m_updateTimer += dt;
		if (m_updateTimer >= m_fpsTime)
		{
			m_updateTimer = 0.f;
			++m_frameCurrent;
			if (m_frameCurrent == m_frameNum)
			{
				if (m_loop)
				{
					m_frameCurrent = 0;
				}
				else
				{
					m_play = false;
				}
			}
		}
	}
}

#endif
