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
#ifndef __BQ_SOUNDMIXER_H__
#define __BQ_SOUNDMIXER_H__

// Класс который смешивает звуки в одно целое.
// Должны быть реализованы эффекты.
// Звуки должны добавляться, возможно в специальную оболочку, так как надо запоминать текущую позицию воспроизведения.

class bqSoundMixer
{
public:
	bqSoundMixer(){}
	virtual ~bqSoundMixer(){}

	//virtual void SetVolume(float) = 0;
	//virtual float GetVolume() = 0;
	// громкость должна быть реализована эффектом
	virtual void AddEffect(bqSoundEffect*) = 0;
	virtual void RemoveEffect(bqSoundEffect*) = 0;
	virtual void RemoveAllEffects() = 0;

	// Звук должен быть формата float32
	virtual void AddSound(bqSound*) = 0;
	virtual void RemoveSound(bqSound*) = 0;
	virtual void RemoveAllSounds() = 0;
};

#endif
