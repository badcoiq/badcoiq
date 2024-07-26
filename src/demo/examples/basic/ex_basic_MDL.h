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
#ifndef _EXmdl_H_
#define _EXmdl_H_

class DemoExample;
class DemoApp;
class ExampleBasicsMDL : public DemoExample
{
	bqCamera* m_camera = 0;


	float m_rotationAngle = 0.f;

	//bqGPUMesh* m_mesh = 0;
	//bqSceneObject* m_sceneObject = 0;
	//bqSkeleton* m_skeleton = 0;
	bqMDL* m_mdl = 0;

	bqTexture* m_texture = 0;

    void _onCamera();
public:
	ExampleBasicsMDL(DemoApp*);
	virtual ~ExampleBasicsMDL();
	BQ_PLACEMENT_ALLOCATOR(ExampleBasicsMDL);

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual void OnDraw() override;
};


#endif
