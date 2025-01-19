/*
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

#include "badcoiq.h"
#include "badcoiq/framework/bqShortcutManager.h"

#include "../framework/bqFrameworkImpl.h"
extern bqFrameworkImpl* g_framework;

bqShortcutManager::bqShortcutManager(uint32_t sz)
{
	for (size_t i = 0; i < sz; ++i)
	{
		m_commands.push_back(new bqShortcutCommand);
	}
}

bqShortcutManager::~bqShortcutManager()
{
	for (size_t i = 0, sz = m_commands.m_size; i < sz; ++i)
	{
		delete m_commands.m_data[i];
	}
}

void bqShortcutManager::SetCommand(
	uint32_t cmd, 
	const wchar_t* commandname, 
	uint32_t keyboardModifier, 
	uint32_t key)
{
	m_commands[cmd]->m_key = key;
	m_commands[cmd]->m_name = commandname;
	m_commands[cmd]->m_keyMod = keyboardModifier;
	m_commands[cmd]->m_text.clear();

	if (key != bqInput::KEY_NONE)
	{
		switch (keyboardModifier)
		{
		case 0:break;
		case bq::KeyboardMod_Ctrl:m_commands[cmd]->m_text += L"Ctrl+"; break;
		case bq::KeyboardMod_Alt:m_commands[cmd]->m_text += L"Alt+"; break;
		case bq::KeyboardMod_Shift:m_commands[cmd]->m_text += L"Shift+"; break;
		case bq::KeyboardMod_ShiftAlt:m_commands[cmd]->m_text += L"Shift+Alt+"; break;
		case bq::KeyboardMod_CtrlShift:m_commands[cmd]->m_text += L"Shift+Ctrl+"; break;
		case bq::KeyboardMod_CtrlShiftAlt:m_commands[cmd]->m_text += L"Shift+Ctrl+Alt+"; break;
		case bq::KeyboardMod_CtrlAlt:m_commands[cmd]->m_text += L"Ctrl+Alt+"; break;
		default:break;
		}

		switch (key)
		{
		case bqInput::KEY_PAUSE:m_commands[cmd]->m_text += L"Pause"; break;
		case bqInput::KEY_SPACE:m_commands[cmd]->m_text += L"Space"; break;
		case bqInput::KEY_PGUP:m_commands[cmd]->m_text += L"PageUp"; break;
		case bqInput::KEY_PGDOWN:m_commands[cmd]->m_text += L"PageDown"; break;
		case bqInput::KEY_END:m_commands[cmd]->m_text += L"End"; break;
		case bqInput::KEY_HOME:m_commands[cmd]->m_text += L"Home"; break;
		case bqInput::KEY_LEFT:m_commands[cmd]->m_text += L"Left"; break;
		case bqInput::KEY_UP:m_commands[cmd]->m_text += L"Up"; break;
		case bqInput::KEY_RIGHT:m_commands[cmd]->m_text += L"Right"; break;
		case bqInput::KEY_DOWN:m_commands[cmd]->m_text += L"Down"; break;
		case bqInput::KEY_PRTSCR:m_commands[cmd]->m_text += L"PrtScr"; break;
		case bqInput::KEY_INSERT:m_commands[cmd]->m_text += L"Insert"; break;
		case bqInput::KEY_0:m_commands[cmd]->m_text += L"0"; break;
		case bqInput::KEY_1:m_commands[cmd]->m_text += L"1"; break;
		case bqInput::KEY_2:m_commands[cmd]->m_text += L"2"; break;
		case bqInput::KEY_3:m_commands[cmd]->m_text += L"3"; break;
		case bqInput::KEY_4:m_commands[cmd]->m_text += L"4"; break;
		case bqInput::KEY_5:m_commands[cmd]->m_text += L"5"; break;
		case bqInput::KEY_6:m_commands[cmd]->m_text += L"6"; break;
		case bqInput::KEY_7:m_commands[cmd]->m_text += L"7"; break;
		case bqInput::KEY_8:m_commands[cmd]->m_text += L"8"; break;
		case bqInput::KEY_9:m_commands[cmd]->m_text += L"9"; break;
		case bqInput::KEY_A:m_commands[cmd]->m_text += L"A"; break;
		case bqInput::KEY_B:m_commands[cmd]->m_text += L"B"; break;
		case bqInput::KEY_C:m_commands[cmd]->m_text += L"C"; break;
		case bqInput::KEY_D:m_commands[cmd]->m_text += L"D"; break;
		case bqInput::KEY_E:m_commands[cmd]->m_text += L"E"; break;
		case bqInput::KEY_F:m_commands[cmd]->m_text += L"F"; break;
		case bqInput::KEY_G:m_commands[cmd]->m_text += L"G"; break;
		case bqInput::KEY_H:m_commands[cmd]->m_text += L"H"; break;
		case bqInput::KEY_I:m_commands[cmd]->m_text += L"I"; break;
		case bqInput::KEY_J:m_commands[cmd]->m_text += L"J"; break;
		case bqInput::KEY_K:m_commands[cmd]->m_text += L"K"; break;
		case bqInput::KEY_L:m_commands[cmd]->m_text += L"L"; break;
		case bqInput::KEY_M:m_commands[cmd]->m_text += L"M"; break;
		case bqInput::KEY_N:m_commands[cmd]->m_text += L"N"; break;
		case bqInput::KEY_O:m_commands[cmd]->m_text += L"O"; break;
		case bqInput::KEY_P:m_commands[cmd]->m_text += L"P"; break;
		case bqInput::KEY_Q:m_commands[cmd]->m_text += L"Q"; break;
		case bqInput::KEY_R:m_commands[cmd]->m_text += L"R"; break;
		case bqInput::KEY_S:m_commands[cmd]->m_text += L"S"; break;
		case bqInput::KEY_T:m_commands[cmd]->m_text += L"T"; break;
		case bqInput::KEY_U:m_commands[cmd]->m_text += L"U"; break;
		case bqInput::KEY_V:m_commands[cmd]->m_text += L"V"; break;
		case bqInput::KEY_W:m_commands[cmd]->m_text += L"W"; break;
		case bqInput::KEY_X:m_commands[cmd]->m_text += L"X"; break;
		case bqInput::KEY_Y:m_commands[cmd]->m_text += L"Y"; break;
		case bqInput::KEY_Z:m_commands[cmd]->m_text += L"Z"; break;
		case bqInput::KEY_NUM_0:m_commands[cmd]->m_text += L"Num0"; break;
		case bqInput::KEY_NUM_1:m_commands[cmd]->m_text += L"Num1"; break;
		case bqInput::KEY_NUM_2:m_commands[cmd]->m_text += L"Num2"; break;
		case bqInput::KEY_NUM_3:m_commands[cmd]->m_text += L"Num3"; break;
		case bqInput::KEY_NUM_4:m_commands[cmd]->m_text += L"Num4"; break;
		case bqInput::KEY_NUM_5:m_commands[cmd]->m_text += L"Num5"; break;
		case bqInput::KEY_NUM_6:m_commands[cmd]->m_text += L"Num6"; break;
		case bqInput::KEY_NUM_7:m_commands[cmd]->m_text += L"Num7"; break;
		case bqInput::KEY_NUM_8:m_commands[cmd]->m_text += L"Num8"; break;
		case bqInput::KEY_NUM_9:m_commands[cmd]->m_text += L"Num9"; break;
		case bqInput::KEY_NUM_MUL:m_commands[cmd]->m_text += L"Num*"; break;
		case bqInput::KEY_NUM_ADD:m_commands[cmd]->m_text += L"Num+"; break;
		case bqInput::KEY_NUM_SUB:m_commands[cmd]->m_text += L"Num-"; break;
		case bqInput::KEY_NUM_DOT:m_commands[cmd]->m_text += L"Num."; break;
		case bqInput::KEY_NUM_SLASH:m_commands[cmd]->m_text += L"Num/"; break;
		case bqInput::KEY_F1:m_commands[cmd]->m_text += L"F1"; break;
		case bqInput::KEY_F2:m_commands[cmd]->m_text += L"F2"; break;
		case bqInput::KEY_F3:m_commands[cmd]->m_text += L"F3"; break;
		case bqInput::KEY_F4:m_commands[cmd]->m_text += L"F4"; break;
		case bqInput::KEY_F5:m_commands[cmd]->m_text += L"F5"; break;
		case bqInput::KEY_F6:m_commands[cmd]->m_text += L"F6"; break;
		case bqInput::KEY_F7:m_commands[cmd]->m_text += L"F7"; break;
		case bqInput::KEY_F8:m_commands[cmd]->m_text += L"F8"; break;
		case bqInput::KEY_F9:m_commands[cmd]->m_text += L"F9"; break;
		case bqInput::KEY_F10:m_commands[cmd]->m_text += L"F10"; break;
		case bqInput::KEY_F11:m_commands[cmd]->m_text += L"F11"; break;
		case bqInput::KEY_F12:m_commands[cmd]->m_text += L"F12"; break;
		case bqInput::KEY_COLON:m_commands[cmd]->m_text += L";"; break;
		case bqInput::KEY_ADD:m_commands[cmd]->m_text += L"="; break;
		case bqInput::KEY_COMMA:m_commands[cmd]->m_text += L","; break;
		case bqInput::KEY_SUB:m_commands[cmd]->m_text += L"-"; break;
		case bqInput::KEY_DOT:m_commands[cmd]->m_text += L"."; break;
		case bqInput::KEY_SLASH:m_commands[cmd]->m_text += L"/"; break;
		case bqInput::KEY_TILDE:m_commands[cmd]->m_text += L"`"; break;
		case bqInput::KEY_FIGURE_OPEN:m_commands[cmd]->m_text += L"["; break;
		case bqInput::KEY_FIGURE_CLOSE:m_commands[cmd]->m_text += L"]"; break;
		case bqInput::KEY_QUOTE:m_commands[cmd]->m_text += L"'"; break;
		default:break;
		}
	}
}

bool bqShortcutManager::IsShortcutActive(uint32_t cmd)
{
	auto command = m_commands[cmd];
	if (command->m_key == bqInput::KEY_NONE)
		return false;

	if (command->m_keyMod != g_framework->m_input.m_keyboardModifier)
		return false;

	return bqInput::IsKeyHit(command->m_key);
}

const char32_t* bqShortcutManager::GetText(uint32_t cmd)
{
	return m_commands[cmd]->m_text.c_str();
}

const wchar_t* bqShortcutManager::GetTextW(uint32_t cmd)
{
	m_w_string.clear();
	m_commands[cmd]->m_text.to_utf16(m_w_string);
	return m_w_string.c_str();
}
