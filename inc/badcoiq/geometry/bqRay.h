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
#ifndef __BQ_RAY_H__
//
#define __BQ_RAY_H__

#include "badcoiq/math/bqMath.h"

/// 
class bqRay
{
public:
	bqRay() {}
	~bqRay() {}

	bqVec3 m_origin;
	bqVec3 m_end;

	/// Так-же полезно иметь дополнительные данные
	bqVec3 m_direction;
	bqVec3 m_invDir;
	bqReal m_segmentLen;

	/// Вычислить m_segmentLen.
	/// Расстояние от m_origin до m_end
	void UpdateSegmentLen();

	/// Для Watertight Ray/Triangle Intersection
	int32_t m_kz = 0;
	int32_t m_kx = 0;
	int32_t m_ky = 0;
	bqReal m_Sx = 0;
	bqReal m_Sy = 0;
	bqReal m_Sz = 0;

	/// Получить луч от экрана по 2D координатам
	/// rc_sz - размер прямоугольнойобласти в пикселях. Это если например делаем редактор
	///         со множеством вьюпортов
	/// VPinv - ViewProjectionInvert
	/// depthRange - у OpenGL и DirectX различная глубина. bqGS должен иметь метод
	///              для получения depth range.
	void CreateFrom2DCoords(
		const bqVec2f& coord,
		const bqVec2f& rc_sz,
		const bqMat4& VPinv,
		const bqVec2f& depthRange);

	/// Пересечение линий.
	/// Установить пересекает ли линия другую линию невозможно так как они не имеют
	///  толщину. Логичнее получить расстояние между ними.
	bqReal DistanceToLine(const bqVec3& lineP0, const bqVec3& lineP1);

	/// После установки m_origin или m_end
	/// Вычислить остальные значения типа m_direction, m_kz
	void Update();

	/// Получить точку на луче, на расстоянии t от начала (m_origin)
	void GetIntersectionPoint(bqReal t, bqVec3& ip);

	/// Пересекает ли плоскость.
	bool PlaneIntersection(const bqVec3& planePoint, const bqVec3& planeNormal, bqReal& T);
};

#endif

