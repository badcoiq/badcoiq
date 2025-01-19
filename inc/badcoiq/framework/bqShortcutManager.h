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
#ifndef _BQSHTMNGR_H_
/// \cond
#define _BQSHTMNGR_H_
/// \endcond

#include "badcoiq/input/bqInputEx.h"
#include "badcoiq/containers/bqArray.h"

struct bqShortcutCommand
{
	//bqShortcutCommandType m_type;
	bqString m_text;
	bqString m_name;
	uint32_t m_keyMod = 0;
	uint32_t m_key = bqInput::KEY_NONE;
};

/// \brief Обработка нажатий типа Ctrl+S
class bqShortcutManager
{
	bqArray<bqShortcutCommand*> m_commands;
	bqStringW m_w_string;
public:

	/// Указываем количество комманд.
	/// Массив m_commands инициализируется.
	bqShortcutManager(uint32_t);
	~bqShortcutManager();
	BQ_PLACEMENT_ALLOCATOR(bqShortcutManager);

	void SetCommand(uint32_t cmd, const wchar_t* commandname, uint32_t keyboardModifier, uint32_t key);
	bool IsShortcutActive(uint32_t cmd);
	const char32_t* GetText(uint32_t cmd);
	const wchar_t* GetTextW(uint32_t cmd);
};


#endif
