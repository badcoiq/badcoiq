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
    
    class MyModel
    {
        class mesh_buffer
        {
        public:
            mesh_buffer() {}
            ~mesh_buffer() 
            {
                if(m_mesh)
                    delete m_mesh;
            }
            
            bqGPUMesh* m_mesh = 0;
            bqTexture* m_texture = 0;
            bqString m_materialName;
        };

        class MyMeshLoaderCallback : public bqMeshLoaderCallback, public bqUserData
        {
        public:
            MyMeshLoaderCallback() {}
            virtual ~MyMeshLoaderCallback() {}

            virtual void OnMaterial(bqMaterial* m) override
            {
                if (m)
                {
                    bqStringA stra;
                    m->m_name.to_utf8(stra);
                    bqLog::Print("MATERIAL %s\n", stra.c_str());

                    MyModel* mesh = (MyModel*)GetUserData();
                    
                    mesh->m_materials.push_back(*m);
                }
            }

            virtual void OnMesh(bqMesh* newMesh, bqString* name, bqString* materialName) override
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
                    mesh_buffer* mb = new mesh_buffer;
                    mb->m_mesh = m->m_gs->CreateMesh(newMesh);
                    m->m_meshBuffers.push_back(mb);
                    mb->m_materialName = *materialName;

                    bqDestroy(newMesh);
                }
            }

            virtual void Finale() override
            {
                MyModel* m = (MyModel*)GetUserData();
                m->OnFinale();
            }
        };
        MyMeshLoaderCallback m_cb;
    public:
        MyModel(bqGS* gs) :m_gs(gs) 
        {
            m_cb.SetUserData(this);
        }

        ~MyModel()
        {
            for (size_t i = 0; i < m_loadedTextures.m_size; ++i)
            {
                delete m_loadedTextures.m_data[i];
            }

            for (size_t i = 0; i < m_meshBuffers.m_size; ++i)
            {
                delete m_meshBuffers.m_data[i];
            }
        }

        void Load(const char* f)
        {
            bqFramework::CreateMesh(f, &m_cb);
        }

        void OnFinale()
        {
            // надо найти материалы по имени, и загрузить текстуры.
            for (size_t i = 0; i < m_meshBuffers.m_size; ++i)
            {
                mesh_buffer* mb = m_meshBuffers.m_data[i];

                bqMaterial* material = GetMaterial(m_meshBuffers.m_data[i]->m_materialName);
                if (material)
                {
                    if (material->m_maps[0].m_filePath)
                    {
                        bqImage* img = bqFramework::CreateImage((const char*)material->m_maps[0].m_filePath);
                        if (img)
                        {
                            bqTextureInfo ti;
                            mb->m_texture = m_gs->CreateTexture(img, ti);
                            m_loadedTextures.push_back(mb->m_texture);
                            delete img;
                        }
                    }
                }
            }
        }

        bqMaterial* GetMaterial(const bqString& name)
        {
            for (size_t i = 0; i < m_materials.m_size; ++i)
            {
                if (m_materials.m_data[i].m_name == name)
                    return &m_materials.m_data[i];
            }

            return 0;
        }

        bqGS* m_gs = 0;

        bqArray<bqMaterial> m_materials;
        bqArray<mesh_buffer*> m_meshBuffers;
        bqArray<bqTexture*> m_loadedTextures;
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
