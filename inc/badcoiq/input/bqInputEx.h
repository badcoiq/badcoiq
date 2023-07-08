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
#ifndef __BQ_INPUTEX_H__
#define __BQ_INPUTEX_H__

#include "badcoiq/input/bqInput.h"

namespace bq
{
	// для m_mouseButtonFlags
	enum
	{
		MouseFlag_LMBDOWN = 0x1,
		MouseFlag_LMBUP = 0x2,
		MouseFlag_RMBDOWN = 0x4,
		MouseFlag_RMBUP = 0x8,
		MouseFlag_MMBDOWN = 0x10,
		MouseFlag_MMBUP = 0x20,
		MouseFlag_X1MBDOWN = 0x40,
		MouseFlag_X1MBUP = 0x80,
		MouseFlag_X2MBDOWN = 0x100,
		MouseFlag_X2MBUP = 0x200,

		MouseFlag_LMBHOLD = 0x400,
		MouseFlag_RMBHOLD = 0x800,
		MouseFlag_MMBHOLD = 0x1000,
		MouseFlag_X1MBHOLD = 0x2000,
		MouseFlag_X2MBHOLD = 0x4000,
	};

	// для m_keyboardModifier
	enum
	{
		KeyboardMod_Clear = 0,
		KeyboardMod_Ctrl,
		KeyboardMod_Shift,
		KeyboardMod_Alt,
		KeyboardMod_CtrlShift,
		KeyboardMod_CtrlAlt,
		KeyboardMod_ShiftAlt,
		KeyboardMod_CtrlShiftAlt,
	};

	// Маски для получения бита
	enum : uint64_t
	{
		KeyboardBitMask_NONE = 0,
		KeyboardBitMask_BACKSPACE = 0x1,
		KeyboardBitMask_TAB = 0x2,
		KeyboardBitMask_ENTER = 0x4,
		KeyboardBitMask_SHIFT = 0x8,
		KeyboardBitMask_CTRL = 0x10,
		KeyboardBitMask_ALT = 0x20,
		KeyboardBitMask_PAUSE = 0x40,
		KeyboardBitMask_CAPS_LOCK = 0x80,
		KeyboardBitMask_ESCAPE = 0x100,
		KeyboardBitMask_SPACE = 0x200,
		KeyboardBitMask_PGUP = 0x400,
		KeyboardBitMask_PGDOWN = 0x800,
		KeyboardBitMask_END = 0x1000,
		KeyboardBitMask_HOME = 0x2000,
		KeyboardBitMask_LEFT = 0x4000,
		KeyboardBitMask_UP = 0x8000,
		KeyboardBitMask_RIGHT = 0x10000,
		KeyboardBitMask_DOWN = 0x20000,
		KeyboardBitMask_PRTSCR = 0x40000,
		KeyboardBitMask_INSERT = 0x80000,
		KeyboardBitMask_DELETE = 0x100000,
		KeyboardBitMask_0 = 0x200000,
		KeyboardBitMask_1 = 0x400000,
		KeyboardBitMask_2 = 0x800000,
		KeyboardBitMask_3 = 0x1000000,
		KeyboardBitMask_4 = 0x2000000,
		KeyboardBitMask_5 = 0x4000000,
		KeyboardBitMask_6 = 0x8000000,
		KeyboardBitMask_7 = 0x10000000,
		KeyboardBitMask_8 = 0x20000000,
		KeyboardBitMask_9 = 0x40000000,
		KeyboardBitMask_A = 0x80000000,
		KeyboardBitMask_B = 0x100000000,
		KeyboardBitMask_C = 0x200000000,
		KeyboardBitMask_D = 0x400000000,
		KeyboardBitMask_E = 0x800000000,
		KeyboardBitMask_F = 0x1000000000,
		KeyboardBitMask_G = 0x2000000000,
		KeyboardBitMask_H = 0x4000000000,
		KeyboardBitMask_I = 0x8000000000,
		KeyboardBitMask_J = 0x10000000000,
		KeyboardBitMask_K = 0x20000000000,
		KeyboardBitMask_L = 0x40000000000,
		KeyboardBitMask_M = 0x80000000000,
		KeyboardBitMask_N = 0x100000000000,
		KeyboardBitMask_O = 0x200000000000,
		KeyboardBitMask_P = 0x400000000000,
		KeyboardBitMask_Q = 0x800000000000,
		KeyboardBitMask_R = 0x1000000000000,
		KeyboardBitMask_S = 0x2000000000000,
		KeyboardBitMask_T = 0x4000000000000,
		KeyboardBitMask_U = 0x8000000000000,
		KeyboardBitMask_V = 0x10000000000000,
		KeyboardBitMask_W = 0x20000000000000,
		KeyboardBitMask_X = 0x40000000000000,
		KeyboardBitMask_Y = 0x80000000000000,
		KeyboardBitMask_Z = 0x100000000000000,
		KeyboardBitMask_WIN = 0x200000000000000,
		KeyboardBitMask_CONTEXT = 0x400000000000000,
		KeyboardBitMask_NUM_0 = 0x800000000000000,
		KeyboardBitMask_NUM_1 = 0x1,
		KeyboardBitMask_NUM_2 = 0x2,
		KeyboardBitMask_NUM_3 = 0x4,
		KeyboardBitMask_NUM_4 = 0x8,
		KeyboardBitMask_NUM_5 = 0x10,
		KeyboardBitMask_NUM_6 = 0x20,
		KeyboardBitMask_NUM_7 = 0x40,
		KeyboardBitMask_NUM_8 = 0x80,
		KeyboardBitMask_NUM_9 = 0x100,
		KeyboardBitMask_NUM_MUL = 0x200, // *
		KeyboardBitMask_NUM_ADD = 0x400, // +
		KeyboardBitMask_NUM_SUB = 0x800, // -
		KeyboardBitMask_NUM_DOT = 0x1000, // .
		KeyboardBitMask_NUM_SLASH = 0x2000, // /
		KeyboardBitMask_F1 = 0x4000,
		KeyboardBitMask_F2 = 0x8000,
		KeyboardBitMask_F3 = 0x10000,
		KeyboardBitMask_F4 = 0x20000,
		KeyboardBitMask_F5 = 0x40000,
		KeyboardBitMask_F6 = 0x80000,
		KeyboardBitMask_F7 = 0x100000,
		KeyboardBitMask_F8 = 0x200000,
		KeyboardBitMask_F9 = 0x400000,
		KeyboardBitMask_F10 = 0x800000,
		KeyboardBitMask_F11 = 0x1000000,
		KeyboardBitMask_F12 = 0x2000000,
		KeyboardBitMask_NUM_LOCK = 0x4000000,
		KeyboardBitMask_SCROLL_LOCK = 0x8000000,
		KeyboardBitMask_LSHIFT = 0x10000000,
		KeyboardBitMask_RSHIFT = 0x20000000,
		KeyboardBitMask_LCTRL = 0x40000000,
		KeyboardBitMask_RCTRL = 0x80000000,
		KeyboardBitMask_LALT = 0x100000000,
		KeyboardBitMask_RALT = 0x200000000,
		KeyboardBitMask_FUNC_MY_COMP = 0x400000000, // Function key. open My computer
		KeyboardBitMask_FUNC_CALC = 0x800000000, // Function key. open Calculator
		KeyboardBitMask_COLON = 0x1000000000, //;:
		KeyboardBitMask_ADD = 0x2000000000, //=+
		KeyboardBitMask_COMMA = 0x4000000000, //,<
		KeyboardBitMask_SUB = 0x8000000000, //-_
		KeyboardBitMask_DOT = 0x10000000000, //.>
		KeyboardBitMask_SLASH = 0x20000000000, // /?
		KeyboardBitMask_TILDE = 0x40000000000, //`~
		KeyboardBitMask_FIGURE_OPEN = 0x80000000000, //[{
		KeyboardBitMask_BACKSLASH = 0x100000000000, //\|
		KeyboardBitMask_FIGURE_CLOSE = 0x200000000000, //]}
		KeyboardBitMask_QUOTE = 0x400000000000, //'"
		KeyboardBitMask_end__,
	};
}

#endif

