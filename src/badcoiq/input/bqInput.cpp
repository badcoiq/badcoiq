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

#include "badcoiq/input/bqInputEx.h"

#include "../framework/bqFrameworkImpl.h"

#ifdef BQ_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "badcoiq/system/bqWindow.h"
#include "badcoiq/system/bqWindowWin32.h"
#endif
extern bqFrameworkImpl* g_framework;

static const uint64_t g_keyToBin[] =
{
	0, //KEY_NONE = 0,
	0, 0, 0, 0, 0, 0, 0,
	bq::KeyboardBitMask_BACKSPACE,	//KEY_BACKSPACE = 8,
	bq::KeyboardBitMask_TAB,	//KEY_TAB = 9,
	0, 0, 0,
	bq::KeyboardBitMask_ENTER, //KEY_ENTER = 13,
	0, 0,
	bq::KeyboardBitMask_SHIFT, //KEY_SHIFT = 16,
	bq::KeyboardBitMask_CTRL, //KEY_CTRL = 17,
	bq::KeyboardBitMask_ALT, //KEY_ALT = 18,
	bq::KeyboardBitMask_PAUSE,//KEY_PAUSE = 19,
	bq::KeyboardBitMask_CAPS_LOCK,//KEY_CAPS_LOCK = 20,
	0, 0, 0, 0, 0, 0,
	bq::KeyboardBitMask_ESCAPE, //KEY_ESCAPE = 27,
	0, 0, 0, 0,
	bq::KeyboardBitMask_SPACE, //KEY_SPACE = 32,
	bq::KeyboardBitMask_PGUP, //KEY_PGUP = 33,
	bq::KeyboardBitMask_PGDOWN, //KEY_PGDOWN = 34,
	bq::KeyboardBitMask_END, //KEY_END = 35,
	bq::KeyboardBitMask_HOME, //KEY_HOME = 36,
	bq::KeyboardBitMask_LEFT, //KEY_LEFT = 37,
	bq::KeyboardBitMask_UP, //KEY_UP = 38,
	bq::KeyboardBitMask_RIGHT, //KEY_RIGHT = 39,
	bq::KeyboardBitMask_DOWN, //KEY_DOWN = 40,
	0, 0, 0,
	bq::KeyboardBitMask_PRTSCR, //KEY_PRTSCR = 44,
	bq::KeyboardBitMask_INSERT, //KEY_INSERT = 45,
	bq::KeyboardBitMask_DELETE, //KEY_DELETE = 46,
	0,
	bq::KeyboardBitMask_0, //KEY_0 = 48,
	bq::KeyboardBitMask_1, //KEY_1 = 49,
	bq::KeyboardBitMask_2, //KEY_2 = 50,
	bq::KeyboardBitMask_3, //KEY_3 = 51,
	bq::KeyboardBitMask_4, //KEY_4 = 52,
	bq::KeyboardBitMask_5, //KEY_5 = 53,
	bq::KeyboardBitMask_6, //KEY_6 = 54,
	bq::KeyboardBitMask_7, //KEY_7 = 55,
	bq::KeyboardBitMask_8, //KEY_8 = 56,
	bq::KeyboardBitMask_9, //KEY_9 = 57,
	0, 0, 0, 0, 0, 0, 0,
	bq::KeyboardBitMask_A, //KEY_A = 65,
	bq::KeyboardBitMask_B, //KEY_B = 66,
	bq::KeyboardBitMask_C, //KEY_C = 67,
	bq::KeyboardBitMask_D, //KEY_D = 68,
	bq::KeyboardBitMask_E, //KEY_E = 69,
	bq::KeyboardBitMask_F, //KEY_F = 70,
	bq::KeyboardBitMask_G, //KEY_G = 71,
	bq::KeyboardBitMask_H, //KEY_H = 72,
	bq::KeyboardBitMask_I, //KEY_I = 73,
	bq::KeyboardBitMask_J, //KEY_J = 74,
	bq::KeyboardBitMask_K, //KEY_K = 75,
	bq::KeyboardBitMask_L, //KEY_L = 76,
	bq::KeyboardBitMask_M, //KEY_M = 77,
	bq::KeyboardBitMask_N, //KEY_N = 78,
	bq::KeyboardBitMask_O, //KEY_O = 79,
	bq::KeyboardBitMask_P, //KEY_P = 80,
	bq::KeyboardBitMask_Q, //KEY_Q = 81,
	bq::KeyboardBitMask_R, //KEY_R = 82,
	bq::KeyboardBitMask_S, //KEY_S = 83,
	bq::KeyboardBitMask_T, //KEY_T = 84,
	bq::KeyboardBitMask_U, //KEY_U = 85,
	bq::KeyboardBitMask_V, //KEY_V = 86,
	bq::KeyboardBitMask_W, //KEY_W = 87,
	bq::KeyboardBitMask_X, //KEY_X = 88,
	bq::KeyboardBitMask_Y, //KEY_Y = 89,
	bq::KeyboardBitMask_Z, //KEY_Z = 90,
	bq::KeyboardBitMask_WIN, //KEY_WIN = 91,
	0,
	bq::KeyboardBitMask_CONTEXT, //KEY_CONTEXT = 93,
	0, 0,
	bq::KeyboardBitMask_NUM_0, //KEY_NUM_0 = 96,
	bq::KeyboardBitMask_NUM_1, //KEY_NUM_1 = 97,
	bq::KeyboardBitMask_NUM_2, //KEY_NUM_2 = 98,
	bq::KeyboardBitMask_NUM_3, //KEY_NUM_3 = 99,
	bq::KeyboardBitMask_NUM_4, //KEY_NUM_4 = 100,
	bq::KeyboardBitMask_NUM_5, //KEY_NUM_5 = 101,
	bq::KeyboardBitMask_NUM_6, //KEY_NUM_6 = 102,
	bq::KeyboardBitMask_NUM_7, //KEY_NUM_7 = 103,
	bq::KeyboardBitMask_NUM_8, //KEY_NUM_8 = 104,
	bq::KeyboardBitMask_NUM_9, //KEY_NUM_9 = 105,
	bq::KeyboardBitMask_NUM_MUL,//	KEY_NUM_MUL = 106, // *
	bq::KeyboardBitMask_NUM_ADD, //KEY_NUM_ADD = 107, // +
	0,
	bq::KeyboardBitMask_NUM_SUB, //KEY_NUM_SUB = 109, // -
	bq::KeyboardBitMask_NUM_DOT, //KEY_NUM_DOT = 110, // .
	bq::KeyboardBitMask_NUM_SLASH, //KEY_NUM_SLASH = 111, // /
	bq::KeyboardBitMask_F1, //KEY_F1 = 112,
	bq::KeyboardBitMask_F2, //KEY_F2 = 113,
	bq::KeyboardBitMask_F3, //	KEY_F3 = 114,
	bq::KeyboardBitMask_F4, //KEY_F4 = 115,
	bq::KeyboardBitMask_F5, //KEY_F5 = 116,
	bq::KeyboardBitMask_F6, //KEY_F6 = 117,
	bq::KeyboardBitMask_F7, //KEY_F7 = 118,
	bq::KeyboardBitMask_F8, //KEY_F8 = 119,
	bq::KeyboardBitMask_F9, //KEY_F9 = 120,
	bq::KeyboardBitMask_F10, //KEY_F10 = 121,
	bq::KeyboardBitMask_F11, //KEY_F11 = 122,
	bq::KeyboardBitMask_F12, //KEY_F12 = 123,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		bq::KeyboardBitMask_NUM_LOCK,//	KEY_NUM_LOCK = 144,
		bq::KeyboardBitMask_SCROLL_LOCK, //KEY_SCROLL_LOCK = 145,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		bq::KeyboardBitMask_LSHIFT, //KEY_LSHIFT = 160,
		bq::KeyboardBitMask_RSHIFT, //KEY_RSHIFT = 161,
		bq::KeyboardBitMask_LCTRL, //KEY_LCTRL = 162,
		bq::KeyboardBitMask_RCTRL, //KEY_RCTRL = 163,
		bq::KeyboardBitMask_LALT, //KEY_LALT = 164,
		bq::KeyboardBitMask_RALT, //KEY_RALT = 165,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		bq::KeyboardBitMask_FUNC_MY_COMP, //KEY_FUNC_MY_COMP = 182, // Function key. open My computer
		bq::KeyboardBitMask_FUNC_CALC, //KEY_FUNC_CALC = 183, // Function key. open Calculator
		0, 0,
		bq::KeyboardBitMask_COLON, //KEY_COLON = 186, //;:
		bq::KeyboardBitMask_ADD, //KEY_ADD = 187, //=+
		bq::KeyboardBitMask_COMMA, //KEY_COMMA = 188, //,<
		bq::KeyboardBitMask_SUB, //KEY_SUB = 189, //-_
		bq::KeyboardBitMask_DOT, //KEY_DOT = 190, //.>
		bq::KeyboardBitMask_SLASH, //KEY_SLASH = 191, // /?
		bq::KeyboardBitMask_TILDE, //KEY_TILDE = 192, //`~
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		bq::KeyboardBitMask_FIGURE_OPEN, //KEY_FIGURE_OPEN = 219, //[{
		bq::KeyboardBitMask_BACKSLASH, //KEY_BACKSLASH = 220, //\|
		bq::KeyboardBitMask_FIGURE_CLOSE, //KEY_FIGURE_CLOSE = 221, //]}
		bq::KeyboardBitMask_QUOTE, //KEY_QUOTE = 222,
};

// обнуляем здесь
void bqInputUpdatePre()
{
	g_framework->m_input.m_mousePositionOld = g_framework->m_input.m_mousePosition;
	g_framework->m_input.m_mouseButtonFlags &= ~0x3FF;
	g_framework->m_input.m_mouseWheelDelta = 0;
	g_framework->m_input.m_character = 0;
	g_framework->m_input.m_keyFlagsHit[0] = 0;
	g_framework->m_input.m_keyFlagsHit[1] = 0;
	g_framework->m_input.m_keyFlagsRelease[0] = 0;
	g_framework->m_input.m_keyFlagsRelease[1] = 0;
}

// тут находим m_mouseMoveDelta и m_keyboardModifier
void bqInputUpdatePost()
{
	unsigned int ctrl_shift_alt = 0;
	if (bqInput::IsKeyHold(bqInput::KEY_LALT) || bqInput::IsKeyHold(bqInput::KEY_RALT))
		ctrl_shift_alt |= 1;

	if (bqInput::IsKeyHold(bqInput::KEY_LSHIFT) || bqInput::IsKeyHold(bqInput::KEY_RSHIFT))
		ctrl_shift_alt |= 2;

	if (bqInput::IsKeyHold(bqInput::KEY_LCTRL) || bqInput::IsKeyHold(bqInput::KEY_RCTRL))
		ctrl_shift_alt |= 4;

	switch (ctrl_shift_alt)
	{
	default:
	case 0:  g_framework->m_input.m_keyboardModifier = bq::KeyboardMod_Clear;         break;
	case 1:  g_framework->m_input.m_keyboardModifier = bq::KeyboardMod_Alt;           break;
	case 2:  g_framework->m_input.m_keyboardModifier = bq::KeyboardMod_Shift;         break;
	case 3:  g_framework->m_input.m_keyboardModifier = bq::KeyboardMod_ShiftAlt;      break;
	case 4:  g_framework->m_input.m_keyboardModifier = bq::KeyboardMod_Ctrl;          break;
	case 5:  g_framework->m_input.m_keyboardModifier = bq::KeyboardMod_CtrlAlt;       break;
	case 6:  g_framework->m_input.m_keyboardModifier = bq::KeyboardMod_CtrlShift;     break;
	case 7:  g_framework->m_input.m_keyboardModifier = bq::KeyboardMod_CtrlShiftAlt;  break;
	}

	g_framework->m_input.m_mouseMoveDelta.x = g_framework->m_input.m_mousePosition.x -
		g_framework->m_input.m_mousePositionOld.x;
	g_framework->m_input.m_mouseMoveDelta.y = g_framework->m_input.m_mousePosition.y -
		g_framework->m_input.m_mousePositionOld.y;
}

bqInputData* bqInput::GetData()
{
	BQ_ASSERT_ST(g_framework);
	return &g_framework->m_input;
}

bqPointf& bqInput::GetMousePosition()
{
	BQ_ASSERT_ST(g_framework);
	return g_framework->m_input.m_mousePosition;
}

void bqInput::SetKeyHit(uint32_t k, bool v)
{
	BQ_ASSERT_ST(g_framework);

	uint64_t bit = g_keyToBin[k];

	if (k < bqInput::KEY_NUM_1)
	{
		if (v)
			g_framework->m_input.m_keyFlagsHit[0] |= bit;
		else
			g_framework->m_input.m_keyFlagsHit[0] &= ~bit;
	}
	else
	{
		if (v)
			g_framework->m_input.m_keyFlagsHit[1] |= bit;
		else
			g_framework->m_input.m_keyFlagsHit[1] &= ~bit;
	}
}

void bqInput::SetKeyHold(uint32_t k, bool v)
{
	BQ_ASSERT_ST(g_framework);

	uint64_t bit = g_keyToBin[k];

	if (k < bqInput::KEY_NUM_1)
	{
		if (v)
			g_framework->m_input.m_keyFlagsHold[0] |= bit;
		else
			g_framework->m_input.m_keyFlagsHold[0] &= ~bit;
	}
	else
	{
		if (v)
			g_framework->m_input.m_keyFlagsHold[1] |= bit;
		else
			g_framework->m_input.m_keyFlagsHold[1] &= ~bit;
	}
}

void bqInput::SetKeyRelease(uint32_t k, bool v)
{
	BQ_ASSERT_ST(g_framework);

	uint64_t bit = g_keyToBin[k];

	if (k < bqInput::KEY_NUM_1)
	{
		if (v)
			g_framework->m_input.m_keyFlagsRelease[0] |= bit;
		else
			g_framework->m_input.m_keyFlagsRelease[0] &= ~bit;
	}
	else
	{
		if (v)
			g_framework->m_input.m_keyFlagsRelease[1] |= bit;
		else
			g_framework->m_input.m_keyFlagsRelease[1] &= ~bit;
	}
}

bool bqInput::IsKeyHit(uint32_t i)
{
	BQ_ASSERT_ST(g_framework);

	uint64_t bit = g_keyToBin[i];

	if (i < bqInput::KEY_NUM_1) // первый uint64_t
		return (g_framework->m_input.m_keyFlagsHit[0] & bit);
	else
		return (g_framework->m_input.m_keyFlagsHit[1] & bit);
}

bool bqInput::IsKeyHold(uint32_t i)
{
	BQ_ASSERT_ST(g_framework);

	uint64_t bit = g_keyToBin[i];

	if (i < bqInput::KEY_NUM_1) // первый uint64_t
		return (g_framework->m_input.m_keyFlagsHold[0] & bit);
	else
		return (g_framework->m_input.m_keyFlagsHold[1] & bit);
}

bool bqInput::IsKeyRelease(uint32_t i)
{
	BQ_ASSERT_ST(g_framework);

	uint64_t bit = g_keyToBin[i];

	if (i < bqInput::KEY_NUM_1) // первый uint64_t
		return (g_framework->m_input.m_keyFlagsRelease[0] & bit);
	else
		return (g_framework->m_input.m_keyFlagsRelease[1] & bit);
}

void bqInput::EnableLMBDown() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_LMBDOWN; }
void bqInput::EnableLMBHold() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_LMBHOLD; }
void bqInput::EnableLMBUp() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_LMBUP; }
void bqInput::EnableRMBDown() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_RMBDOWN; }
void bqInput::EnableRMBHold() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_RMBHOLD; }
void bqInput::EnableRMBUp() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_RMBUP; }
void bqInput::EnableMMBDown() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_MMBDOWN; }
void bqInput::EnableMMBHold() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_MMBHOLD; }
void bqInput::EnableMMBUp() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_MMBUP; }
void bqInput::EnableX1MBDown() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_X1MBDOWN; }
void bqInput::EnableX1MBHold() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_X1MBHOLD; }
void bqInput::EnableX1MBUp() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_X1MBUP; }
void bqInput::EnableX2MBDown() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_X2MBDOWN; }
void bqInput::EnableX2MBHold() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_X2MBHOLD; }
void bqInput::EnableX2MBUp() { g_framework->m_input.m_mouseButtonFlags |= bq::MouseFlag_X2MBUP; }

void bqInput::DisableLMBDown() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_LMBDOWN; }
void bqInput::DisableLMBHold() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_LMBHOLD; }
void bqInput::DisableLMBUp() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_LMBUP; }
void bqInput::DisableRMBDown() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_RMBDOWN; }
void bqInput::DisableRMBHold() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_RMBHOLD; }
void bqInput::DisableRMBUp() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_RMBUP; }
void bqInput::DisableMMBDown() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_MMBDOWN; }
void bqInput::DisableMMBHold() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_MMBHOLD; }
void bqInput::DisableMMBUp() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_MMBUP; }
void bqInput::DisableX1MBDown() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_X1MBDOWN; }
void bqInput::DisableX1MBHold() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_X1MBHOLD; }
void bqInput::DisableX1MBUp() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_X1MBUP; }
void bqInput::DisableX2MBDown() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_X2MBDOWN; }
void bqInput::DisableX2MBHold() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_X2MBHOLD; }
void bqInput::DisableX2MBUp() { g_framework->m_input.m_mouseButtonFlags &= ~bq::MouseFlag_X2MBUP; }

bool bqInput::IsLMBHit() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBDOWN) == bq::MouseFlag_LMBDOWN; }
bool bqInput::IsLMBHold() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBHOLD) == bq::MouseFlag_LMBHOLD; }
bool bqInput::IsLMBRelease() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_LMBUP) == bq::MouseFlag_LMBUP; }
bool bqInput::IsRMBHit() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_RMBDOWN) == bq::MouseFlag_RMBDOWN; }
bool bqInput::IsRMBHold() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_RMBHOLD) == bq::MouseFlag_RMBHOLD; }
bool bqInput::IsRMBRelease() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_RMBUP) == bq::MouseFlag_RMBUP; }
bool bqInput::IsMMBHit() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_MMBDOWN) == bq::MouseFlag_MMBDOWN; }
bool bqInput::IsMMBHold() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_MMBHOLD) == bq::MouseFlag_MMBHOLD; }
bool bqInput::IsMMBRelease() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_MMBUP) == bq::MouseFlag_MMBUP; }
bool bqInput::IsX1MBHit() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_X1MBDOWN) == bq::MouseFlag_X1MBDOWN; }
bool bqInput::IsX1MBHold() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_X1MBHOLD) == bq::MouseFlag_X1MBHOLD; }
bool bqInput::IsX1MBRelease() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_X1MBUP) == bq::MouseFlag_X1MBUP; }
bool bqInput::IsX2MBHit() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_X2MBDOWN) == bq::MouseFlag_X2MBDOWN; }
bool bqInput::IsX2MBHold() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_X2MBHOLD) == bq::MouseFlag_X2MBHOLD; }
bool bqInput::IsX2MBRelease() { return (g_framework->m_input.m_mouseButtonFlags & bq::MouseFlag_X2MBUP) == bq::MouseFlag_X2MBUP; }


//static const uint64_t* GetKeyToBin()
//{
//	return g_keyToBin;
//}

void bqInput::SetMousePosition(bqWindow* w, int32_t x, int32_t y)
{

#ifdef BQ_PLATFORM_WINDOWS
	SetCursorPos(x, y);
	
	g_framework->m_input.m_mousePosition.x = (float)(x - w->GetData()->m_position.x) - w->GetData()->m_borderSize.x;
	g_framework->m_input.m_mousePosition.y = (float)(y - w->GetData()->m_position.y) - w->GetData()->m_borderSize.y;

	//printf("%i %i : %f %f\n", x, y, g_framework->m_input.m_mousePosition.x, g_framework->m_input.m_mousePosition.y);
#else
#error Need implementation
#endif
}

bool bqInput::IsAlt()
{
	return g_framework->m_input.m_keyboardModifier == bq::KeyboardMod_Alt;
}

bool bqInput::IsCtrl()
{
	return g_framework->m_input.m_keyboardModifier == bq::KeyboardMod_Ctrl;
}

bool bqInput::IsShift()
{
	return g_framework->m_input.m_keyboardModifier == bq::KeyboardMod_Shift;
}

bool bqInput::IsShiftAlt()
{
	return g_framework->m_input.m_keyboardModifier == bq::KeyboardMod_ShiftAlt;
}

bool bqInput::IsCtrlAlt()
{
	return g_framework->m_input.m_keyboardModifier == bq::KeyboardMod_CtrlAlt;
}

bool bqInput::IsCtrlShift()
{
	return g_framework->m_input.m_keyboardModifier == bq::KeyboardMod_CtrlShift;
}

bool bqInput::IsCtrlShiftAlt()
{
	return g_framework->m_input.m_keyboardModifier == bq::KeyboardMod_CtrlShiftAlt;
}

