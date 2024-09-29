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
#ifndef __BQ_TRI_H__
#define __BQ_TRI_H__

#include "badcoiq/math/bqMath.h"

class bqTriangle
{
public:
	bqTriangle();
	bqTriangle(const bqVec3& _v1, const bqVec3& _v2, const bqVec3& _v3);
	bqTriangle(const bqVec3f& _v1, const bqVec3f& _v2, const bqVec3f& _v3);

	bqVec3 v1;
	bqVec3 v2;
	bqVec3 v3;
	bqVec3 e1;
	bqVec3 e2;
	bqVec3 normal;

	void Update();
	void Center(bqVec3& out);
	bool SphereIntersect(bqReal radius, const bqVec3& origin, bqReal& T, bqVec3& ip);
	bool RayIntersect_MT(const bqRay& ray, bool withBackFace, bqReal& T, bqReal& U, bqReal& V, bqReal& W);
	bool RayIntersect_Watertight(const bqRay& ray, bool withBackFace, bqReal& T, bqReal& U, bqReal& V, bqReal& W);
	bool TriangleIntersect(const bqVec3& _v1, const bqVec3& _v2, const bqVec3& _v3);
	bool TriangleIntersect(bqTriangle* t);
};

#endif

