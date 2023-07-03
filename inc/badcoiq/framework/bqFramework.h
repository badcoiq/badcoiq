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
#ifndef __BQ_FRAMEWORK_H__
#define __BQ_FRAMEWORK_H__

// Предполагаю что фреймворк должен посылать какие-то сообщения.
// Я это ещё не реализовал
class bqFrameworkCallback
{
public:
	bqFrameworkCallback() {}
	virtual ~bqFrameworkCallback() {}

	virtual void OnMessage() = 0;
};

// API для фреймворка.
class bqFramework
{
public:
	// Когда нам нужно запустить ДВИЖЁК, вызываем Start
	// Он выделит память для внутреннего класса
	static void Start(bqFrameworkCallback*);

	// для завершения работы, он освободит ту память
	static void Stop();

	// Например, он вычислит DeltaTime, обновит состояния ввода, GUI, окна.
	static void Update();

	static float* GetDeltaTime();

	static bqWindow* SummonWindow(bqWindowCallback*);
};

#endif

