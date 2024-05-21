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
#ifndef __BQ_IMAGE_H__
#define __BQ_IMAGE_H__

// GS использует только r8g8b8a8 остальные форматы это смотря какой файл был загружен
// перед использованием как текстура её надо конвертировать в r8g8b8a8
enum class bqImageFormat : uint32_t
{
	r8g8b8,
	r8g8b8a8,
	x1r5g5b5,
	a4r4g4b4,
	x4r4g4b4,
	r5g6b5,
	a1r5g5b5
};

// информация об изображении
struct bqImageInfo
{
	uint32_t m_width = 0;
	uint32_t m_height = 0;
	uint32_t m_bits = 32;
	uint32_t m_pitch = 0;
	bqImageFormat m_format = bqImageFormat::r8g8b8a8;
};

enum class bqImageFillType
{
	// Params in ""

	Solid,
	HorizontalLines, // "int int" - thickness and space between lines
	VerticalLines, // "int int" - thickness and space between lines
	DiagonalLines, // "int int" - thickness and space between lines

	// NOT IMPLEMENTED
	Grid, // "int int int" - thickness and space between lines x and y
	DiagonalGrid, // "int int int" - thickness and space between lines x and y
	Percent, // "int int" - percent from 0 to 100, thickness
	Dashed, // "int int int" - thickness and space between lines, direction (1 - 4)
	ZigZag, // "int" - thickness
	Wave, // "int" - thickness
	Brick, // "int int int" - size x and y, line thickness
	DottedGrid, // "int" - cell size
};

// Image это просто класс с буфером и некоторой информацией.
class bqImage
{
public:
	bqImage();
	~bqImage();
	BQ_PLACEMENT_ALLOCATOR(bqImage);

	uint8_t* m_data = 0;
	uint32_t m_dataSize = 0;
	bqImageInfo m_info;

// быстрое создание 
	// fast creating
	void Create(uint32_t x, uint32_t y);

// все методы ниже должны вызываться только когда image создан.
// проверок нет
// только для r8g8b8a8
	// All methods below is for created image. Image must be created.
	// And only for r8g8b8a8.
	void FlipVertical();
	void FlipPixel();
	void Fill(bqImageFillType, const bqColor& color1, const bqColor& color2,
		const char* params = 0, bqRect* rect = 0);

// реализовано не всё. ДОЛЖНА быть реализована конвертация в r8g8b8a8
	// not all implemented. at least must be implemented conversion into r8g8b8a8
	void ConvertTo(bqImageFormat);

// заполнить m_data используя палитру
// значения палитры 8битные
// ширина и высота w h
// проверок нет. проход по data в соответствии с шириной и высотой
// where... указывает в какое место копировать данные. перехода на строку ниже нет
	void Fill(bqColor* palette, uint8_t* data, uint32_t w, uint32_t h, uint32_t whereX, uint32_t whereY);

	// Only for r8g8b8a8
	void Resize(uint32_t newWidth, uint32_t newHeight, bool useFilter);
};

#endif

