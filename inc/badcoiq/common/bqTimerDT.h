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
#ifndef __BQ_TIMERDT_H__
/// \cond
#define __BQ_TIMERDT_H__
/// \eddcond

/// Таймер. Используется delta time для приращения времени.
class bqTimerDT
{
	float32_t* m_dt = 0;
	float32_t m_limit = 1.f;
	float32_t m_time = 0.f;
public:
	bqTimerDT(float32_t* dt, float32_t limit)
		:
		m_dt(dt),
		m_limit(limit)
	{
	}

	bqTimerDT(float32_t limit);

	float32_t Time() { return m_time; }

	void SetLimit(float32_t l) { m_limit = l; }

	void Update()
	{
		m_time += *m_dt;
		if (m_time >= m_limit)
		{
			m_isTime = true;

			if (m_infinite)
				m_time = 0;
		}
		else
		{
			m_isTime = false;
		}
	}

	void Reset()
	{
		m_time = 0.f;
		m_isTime = false;
	}

	bool m_infinite = true;	
	bool m_isTime = false;
};

#endif

