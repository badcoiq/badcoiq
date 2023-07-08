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
#ifndef __BQ_INPUT_H__
#define __BQ_INPUT_H__


struct bqInputData
{
	// Позиция курсора
	bqPointf m_mousePosition;

	// старая позиция курсора. для вычисления m_mouseMoveDelta
	bqPointf m_mousePositionOld;

	// На сколько переместился курсор
	bqPointf m_mouseMoveDelta;

	// На сколько покрутили колесо мыши
	float   m_mouseWheelDelta = 0.f;

	enum {
		MBFL_LMBDOWN = 0x1,
		MBFL_LMBUP = 0x2,
		MBFL_RMBDOWN = 0x4,
		MBFL_RMBUP = 0x8,
		MBFL_MMBDOWN = 0x10,
		MBFL_MMBUP = 0x20,
		MBFL_X1MBDOWN = 0x40,
		MBFL_X1MBUP = 0x80,
		MBFL_X2MBDOWN = 0x100,
		MBFL_X2MBUP = 0x200,

		MBFL_LMBHOLD = 0x400,
		MBFL_RMBHOLD = 0x800,
		MBFL_MMBHOLD = 0x1000,
		MBFL_X1MBHOLD = 0x2000,
		MBFL_X2MBHOLD = 0x4000,
	};
	uint32_t m_mouseButtonFlags = 0;

	char32_t m_character = 0;

	uint64_t m_keyFlagsHit[2] = { 0,0 };
	uint64_t m_keyFlagsHold[2] = { 0,0 };
	uint64_t m_keyFlagsRelease[2] = { 0,0 };

	/*keyboardModifier == KBMOD_SHIFT*/
	enum {
		KBMOD_clear = 0,
		KBMOD_CTRL,
		KBMOD_SHIFT,
		KBMOD_ALT,
		KBMOD_CTRLSHIFT,
		KBMOD_CTRLALT,
		KBMOD_SHIFTALT,
		KBMOD_CTRLSHIFTALT,
	};

	uint8_t m_keyboardModifier = 0; /*KBMOD...*/
};

#endif

