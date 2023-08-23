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
#ifndef __BQ_BASICTYPES_H__
#define __BQ_BASICTYPES_H__

template<typename T>
class bqPoint_t
{
public:
	bqPoint_t() {}
	bqPoint_t(T X, T Y) : x(X), y(Y) {}

	T x = 0;
	T y = 0;
};

using bqPoint = bqPoint_t<int32_t>;
using bqPointf = bqPoint_t<float>;

template<typename T>
class bqRect_t
{
public:
	bqRect_t() {}
	bqRect_t(T Left, T Top, T Right, T Bottom) :
		left(Left),
		top(Top),
		right(Right),
		bottom(Bottom)
	{}

	T left = 0;
	T top = 0;
	T right = 0;
	T bottom = 0;

	T Width() { return right - left; }
	T Height() { return bottom - top; }

	using _type = T;
};

using bqRect = bqRect_t<int32_t>;
using bqRectf = bqRect_t<float>;

#endif

