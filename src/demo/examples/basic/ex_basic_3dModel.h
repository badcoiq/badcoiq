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
#ifndef _EX3dmod_H_
#define _EX3dmod_H_

#include "badcoiq/geometry/bqMeshLoader.h"

class DemoExample;
class DemoApp;
class ExampleBasics3DModel : public DemoExample
{
	bqCamera* m_camera = 0;
    
    class MyModel;

    class MyMeshLoaderCallback : public bqMeshLoaderCallback, public bqUserData
    {
    public:
        MyMeshLoaderCallback() {}
        virtual ~MyMeshLoaderCallback() {}

        virtual void OnMaterial(bqMaterial* m, bqString* name)
        {
            if (name)
            {
                bqStringA stra;
                name->to_utf8(stra);
                bqLog::Print("MATERIAL %s\n", stra.c_str());
            }
        }

        virtual void OnMesh(bqMesh* newMesh, bqString* name, bqString* materialName)
        {
            if (newMesh)
            {
                if (name)
                {
                    bqStringA stra;
                    name->to_utf8(stra);
                    bqLog::Print("MESH %s\n", stra.c_str());
                }

                MyModel* m = (MyModel*)GetUserData();
                m->m_gpuModels.push_back(m->m_gs->SummonMesh(newMesh));

                bqDestroy(newMesh);
            }
        }
    };

    class MyModel
    {
        MyMeshLoaderCallback m_cb;
    public:
        MyModel(bqGS* gs) :m_gs(gs) 
        {
            m_cb.SetUserData(this);
        }

        ~MyModel()
        {
            for (size_t i = 0; i < m_gpuModels.m_size; ++i)
            {
                delete m_gpuModels.m_data[i];
            }
        }

        void Load(const char* f);

        bqGS* m_gs = 0;

        bqArray<bqGPUMesh*> m_gpuModels;
    };

    MyModel* m_model = 0;
public:
	ExampleBasics3DModel(DemoApp*);
	virtual ~ExampleBasics3DModel();
	BQ_PLACEMENT_ALLOCATOR(ExampleBasics3DModel);

	virtual bool Init() override;
	virtual void Shutdown() override;
	virtual void OnDraw() override;
};

#endif
