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

/* \file bqCryptography
*  
*/

#pragma once
#ifndef __BQ_Cryptography_H__
/// \cond
#define __BQ_Cryptography_H__
/// \endcond



/// \struct bqMD5
///	\brief Структура хранит MD5 хеш
///	
///	Для того чтобы не делать большой класс,
/// сравнение будет реализовано методом в 
/// классе \a bqCryptography.
struct bqMD5
{
	uint32_t m_a = 0;
	uint32_t m_b = 0;
	uint32_t m_c = 0;
	uint32_t m_d = 0;
};

///  \class bqCryptography
///  \brief Набор методов криптографической тематики
class bqCryptography
{
public:

	/// \brief Вычислить md5.
	/// @param b - входной буфер
	/// @param sz - размер буфера
	static bqMD5 MD5(void* b, uint32_t sz);
	
	/// \brief Сравнение md5.
	static bool Compare(const bqMD5&, const bqMD5&);

};

#endif

