﻿/*
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
#pragma once
#ifndef _EXsnd3d_H_
#define _EXsnd3d_H_

class DemoExample;
class DemoApp;
class ExampleSound3D : public DemoExample
{
	bqCamera* m_camera = 0;
	bqSprite* m_spriteSpeaker = 0;
	bqArray<bqTexture*> m_loadedTextures;
	bqSound* m_sound = 0;
	bqSoundMixer* m_mixer = 0;
public:
	ExampleSound3D(DemoApp*);
	virtual ~ExampleSound3D();
	BQ_PLACEMENT_ALLOCATOR(ExampleSound3D);

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual void OnDraw() override;
};

#endif