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
#ifndef _EXOclCul_H_
#define _EXOclCul_H_

#include "badcoiq/scene/bqSceneObject.h"

class SceneObject_ExampleOcclusionCulling : public bqSceneObject
{
public:
	SceneObject_ExampleOcclusionCulling() {}
	virtual ~SceneObject_ExampleOcclusionCulling()
	{
		BQ_SAFEDESTROY(m_occlusionObject);
	}

	bqGPUOcclusionObject* m_occlusionObject = 0;
	bqMat4 WVP;

	bqReal m_distanceToCamera = 0.;
	bqGPUMesh* m_occluderMesh = 0;
	bqGPUMesh* m_drawMesh = 0;
};

class DemoExample;
class DemoApp;
class ExampleBasicsOclCul : public DemoExample
{
	bqCamera* m_camera = 0;

	bqGPUMesh* m_cube = 0;
	bqGPUMesh* m_sphere = 0;
	bqGPUMesh* m_wall = 0;

	bqString m_textNumVisObjs;

	SceneObject_ExampleOcclusionCulling* m_objects[1000];
	bqArray<SceneObject_ExampleOcclusionCulling*> m_objectsInFrustum;
	bqArray<SceneObject_ExampleOcclusionCulling*> m_objectsVisible;

	SceneObject_ExampleOcclusionCulling* m_wallObject = 0;

    void _onCamera();
public:
	ExampleBasicsOclCul(DemoApp*);
	virtual ~ExampleBasicsOclCul();
	BQ_PLACEMENT_ALLOCATOR(ExampleBasicsOclCul);

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual void OnDraw() override;
};

#endif
