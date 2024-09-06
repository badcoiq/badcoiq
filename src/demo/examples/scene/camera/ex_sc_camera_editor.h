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
#ifndef _EXsccamed_H_
#define _EXsccamed_H_


//class miViewportCamera
//{
//public:
//	miViewportCamera() {}
//	~miViewportCamera() {}
//
//	// calculate matrices
//	void Update();
//	bqMat4 m_viewMatrix;
//	bqMat4 m_viewMatrixInvert;
//
//	bqMat4 m_projectionMatrixOrtho;
//	bqMat4 m_projectionMatrixPersp;
//	bqMat4 m_projectionMatrix;
//
//	bqMat4 m_viewProjectionMatrix;
//	bqMat4 m_viewProjectionInvertMatrix;
//
//
//	float32_t m_near = 0.1f;
//	float32_t m_far = 100.f;
//	float32_t m_fov = 1.4f;
//	float32_t m_aspect = 800.f/600.f;
//
//	bqVec3f m_rotationPlatform;
//	bqVec4f m_positionPlatform = bqVec4f(0,0,0,12); // w = height, zoom, mouse wheel value
//	bqVec3f m_positionCamera; // in world
//
//	void Reset();
//	void MoveToSelection();
//
//	void PanMove();
//	void Rotate(float32_t x, float32_t y, float32_t dt);
//	void RotateZ();
//	void Zoom();
//	void ChangeFOV();
//
//};

class DemoExample;
class DemoApp;
class ExampleSceneCameraEdtr : public DemoExample
{
	//miViewportCamera* m_camera = 0;
	bqCamera* m_camera = 0;
public:
	ExampleSceneCameraEdtr(DemoApp*);
	virtual ~ExampleSceneCameraEdtr();
	BQ_PLACEMENT_ALLOCATOR(ExampleSceneCameraEdtr);

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual void OnDraw() override;
};

#endif
