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

#pragma once
#ifndef __BQ_INPUT_H__
#define __BQ_INPUT_H__

// Информация о состоянии клавиатуры и мышки
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

	// Флаги для мышки
	uint32_t m_mouseButtonFlags = 0;

	// При вводе символа он сохраняется сюда
	char32_t m_character = 0;

	// состояния для кнопок клаиатуры
	uint64_t m_keyFlagsHit[2] = { 0,0 };
	uint64_t m_keyFlagsHold[2] = { 0,0 };
	uint64_t m_keyFlagsRelease[2] = { 0,0 };

	// Модификаторы клавиатуры (моя терминология)
	// см bqInputEx.h -> bq::KeyboardMod_
	uint8_t m_keyboardModifier = 0;
};

/// \brief Класс с помощью которого можно узнать о состоянии клавиатуры и мышки
class bqInput
{
public:

	/// \brief Получить bqInputData
	static bqInputData* GetData();

	/// \brief Получить позицию курсора
	static bqPointf& GetMousePosition();

#ifdef BQ_WITH_WINDOW
	/// \brief Установить позицию курсора
	static void SetMousePosition(bqWindow*, int32_t x, int32_t y);
#endif

	static void SetKeyHit(uint32_t, bool);
	static void SetKeyHold(uint32_t, bool);
	static void SetKeyRelease(uint32_t, bool);

	static bool IsKeyHit(uint32_t);
	static bool IsKeyHold(uint32_t);
	static bool IsKeyRelease(uint32_t);

	static bool IsAlt();
	static bool IsCtrl();
	static bool IsShift();
	static bool IsShiftAlt();
	static bool IsCtrlAlt();
	static bool IsCtrlShift();
	static bool IsCtrlShiftAlt();

	static void EnableLMBDown();
	static void EnableLMBHold();
	static void EnableLMBUp();
	static void EnableRMBDown();
	static void EnableRMBHold();
	static void EnableRMBUp();
	static void EnableMMBDown();
	static void EnableMMBHold();
	static void EnableMMBUp();
	static void EnableX1MBDown();
	static void EnableX1MBHold();
	static void EnableX1MBUp();
	static void EnableX2MBDown();
	static void EnableX2MBHold();
	static void EnableX2MBUp();

	static void DisableLMBDown();
	static void DisableLMBHold();
	static void DisableLMBUp();
	static void DisableRMBDown();
	static void DisableRMBHold();
	static void DisableRMBUp();
	static void DisableMMBDown();
	static void DisableMMBHold();
	static void DisableMMBUp();
	static void DisableX1MBDown();
	static void DisableX1MBHold();
	static void DisableX1MBUp();
	static void DisableX2MBDown();
	static void DisableX2MBHold();
	static void DisableX2MBUp();

	static bool IsLMBHit();
	static bool IsLMBHold();
	static bool IsLMBRelease();
	static bool IsRMBHit();
	static bool IsRMBHold();
	static bool IsRMBRelease();
	static bool IsMMBHit();
	static bool IsMMBHold();
	static bool IsMMBRelease();
	static bool IsX1MBHit();
	static bool IsX1MBHold();
	static bool IsX1MBRelease();
	static bool IsX2MBHit();
	static bool IsX2MBHold();
	static bool IsX2MBRelease();

	static const uint64_t* GetKeyToBin();

	/// keyboard button code
	enum : uint32_t {
		KEY_NONE = 0,
		KEY_BACKSPACE = 8,
		KEY_TAB = 9,
		KEY_ENTER = 13,
		KEY_SHIFT = 16,
		KEY_CTRL = 17,
		KEY_ALT = 18,
		KEY_PAUSE = 19,
		KEY_CAPS_LOCK = 20,
		KEY_ESCAPE = 27,
		KEY_SPACE = 32,
		KEY_PGUP = 33,
		KEY_PGDOWN = 34,
		KEY_END = 35,
		KEY_HOME = 36,
		KEY_LEFT = 37,
		KEY_UP = 38,
		KEY_RIGHT = 39,
		KEY_DOWN = 40,
		KEY_PRTSCR = 44,
		KEY_INSERT = 45,
		KEY_DELETE = 46,
		KEY_0 = 48,
		KEY_1 = 49,
		KEY_2 = 50,
		KEY_3 = 51,
		KEY_4 = 52,
		KEY_5 = 53,
		KEY_6 = 54,
		KEY_7 = 55,
		KEY_8 = 56,
		KEY_9 = 57,
		KEY_A = 65,
		KEY_B = 66,
		KEY_C = 67,
		KEY_D = 68,
		KEY_E = 69,
		KEY_F = 70,
		KEY_G = 71,
		KEY_H = 72,
		KEY_I = 73,
		KEY_J = 74,
		KEY_K = 75,
		KEY_L = 76,
		KEY_M = 77,
		KEY_N = 78,
		KEY_O = 79,
		KEY_P = 80,
		KEY_Q = 81,
		KEY_R = 82,
		KEY_S = 83,
		KEY_T = 84,
		KEY_U = 85,
		KEY_V = 86,
		KEY_W = 87,
		KEY_X = 88,
		KEY_Y = 89,
		KEY_Z = 90,
		KEY_WIN = 91,
		KEY_CONTEXT = 93,
		KEY_NUM_0 = 96,
		KEY_NUM_1 = 97,
		KEY_NUM_2 = 98,
		KEY_NUM_3 = 99,
		KEY_NUM_4 = 100,
		KEY_NUM_5 = 101,
		KEY_NUM_6 = 102,
		KEY_NUM_7 = 103,
		KEY_NUM_8 = 104,
		KEY_NUM_9 = 105,
		KEY_NUM_MUL = 106, // *
		KEY_NUM_ADD = 107, // +
		KEY_NUM_SUB = 109, // -
		KEY_NUM_DOT = 110, // .
		KEY_NUM_SLASH = 111, // /
		KEY_F1 = 112,
		KEY_F2 = 113,
		KEY_F3 = 114,
		KEY_F4 = 115,
		KEY_F5 = 116,
		KEY_F6 = 117,
		KEY_F7 = 118,
		KEY_F8 = 119,
		KEY_F9 = 120,
		KEY_F10 = 121,
		KEY_F11 = 122,
		KEY_F12 = 123,
		KEY_NUM_LOCK = 144,
		KEY_SCROLL_LOCK = 145,
		KEY_LSHIFT = 160,
		KEY_RSHIFT = 161,
		KEY_LCTRL = 162,
		KEY_RCTRL = 163,
		KEY_LALT = 164,
		KEY_RALT = 165,
		KEY_FUNC_MY_COMP = 182, // Function key. open My computer
		KEY_FUNC_CALC = 183, // Function key. open Calculator
		KEY_COLON = 186, //;:
		KEY_ADD = 187, //=+
		KEY_COMMA = 188, //,<
		KEY_SUB = 189, //-_
		KEY_DOT = 190, //.>
		KEY_SLASH = 191, // /?
		KEY_TILDE = 192, //`~
		KEY_FIGURE_OPEN = 219, //[{
		KEY_BACKSLASH = 220, //\|
		KEY_FIGURE_CLOSE = 221, //]}
		KEY_QUOTE = 222, //'"
		KEY_end__,
	};
};

#endif

