#include "badcoiq.h"
#include "badcoiq/system/bqWindow.h"
#include "badcoiq/input/bqInput.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/math/bqMath.h"
#include "badcoiq/containers/bqList.h"
#include "badcoiq/containers/bqArray.h"

#include "badcoiq/geometry/bqMeshLoader.h"
#include "badcoiq/archive/bqArchive.h"

#include "badcoiq/scene/bqCamera.h"
#include <list>
BQ_LINK_LIBRARY("badcoiq");

class bqFrameworkCallbackCB : public bqFrameworkCallback
{
public:
    bqFrameworkCallbackCB() {}
    virtual ~bqFrameworkCallbackCB() {}
    virtual void OnMessage() {}
};

bool g_run = true;

class bqWindowCallbackCB : public bqWindowCallback
{
public:
    bqWindowCallbackCB() {}
    virtual ~bqWindowCallbackCB() {}

    virtual void OnClose(bqWindow*)
    {
        printf("Q\n");
        g_run = false;
    }
};

class meshLoaderCallback : public bqMeshLoaderCallback, public bqUserData
{
public:
    meshLoaderCallback() {}
    virtual ~meshLoaderCallback() {}

    virtual void OnMaterial(bqMaterial* m, bqString* name)
    {
        if (name)
        {
            bqStringA stra;
            name->to_utf8(stra);
            bqLog::Print("MATERIAL %s\n", stra.c_str());
        }
    }

    virtual void OnMesh(bqMesh* newMesh, bqString* name, bqString* materialName);
};

class MyModel
{
public:
    MyModel(bqGS* gs) :m_gs(gs) {}
    ~MyModel()
    {
        for (size_t i = 0; i < m_gpuModels.m_size; ++i)
        {
            delete m_gpuModels.m_data[i];
        }
    }

    bqGS* m_gs = 0;
    meshLoaderCallback m_cb;

    bqArray<bqGPUMesh*> m_gpuModels;

    void Load(const char* f)
    {
        m_cb.SetUserData(this);
        bqFramework::SummonMesh(f, &m_cb);
    }
};

void meshLoaderCallback::OnMesh(bqMesh* newMesh, bqString* name, bqString* materialName)
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


#include <Windows.h>

int main()
{

    bqFrameworkCallbackCB fcb;
    bqFramework::Start(&fcb);

    bqWindowCallbackCB wcb;
    auto window = bqFramework::SummonWindow(&wcb);
    if (window && bqFramework::GetGSNum())
    {
        window->SetPositionAndSize(10, 10, 800, 600);
        window->SetVisible(true);

        bqArchiveSystem::ZipAdd("../media/data.zip");

        bqGS* gs = bqFramework::SummonGS(bqFramework::GetGSUID(0));
        if (gs)
        {
            if (gs->Init(window, 0))
            {
                gs->SetClearColor(0.3f, 0.3f, 0.3f, 1.f);

                bqCamera camera;
                camera.m_position.Set(8.f, 8.f, 0.0f, 0.f);
                camera.m_aspect = 300.f / 200.f;
                camera.m_fov = 1.1;
                
                camera.SetType(bqCamera::Type::PerspectiveLookAt);

                camera.m_rotation.Set(bqMath::DegToRad(-75.f), 0.0, 0.f);
                camera.Update(*bqFramework::GetDeltaTime());
                
               // bqMath::LookAtRH(camera.m_view, camera.m_position, bqVec4(), bqVec4(0.f, 1.f, 0.f, 0.f));
                bqMat4 ViewProjection = camera.m_projection * camera.m_view;
                bqFramework::SetMatrix(bqMatrixType::ViewProjection, &ViewProjection);
                
                MyModel* model = new MyModel(gs);
                model->Load(bqFramework::GetPath("../media/1.obj").c_str());
                
                bqImage* image = bqFramework::SummonImage(bqFramework::GetPath("../media/image.bmp").c_str());
                bqTextureInfo ti;
                ti.m_filter = bqTextureFilter::PPP;
                bqTexture* texture = gs->SummonTexture(image, ti);
                delete image;

                bqMaterial m;
                m.m_sunPosition.Set(1.f, 3.f, -1.f);
                m.m_sunPosition.Normalize();
                m.m_shaderType = bqShaderType::Standart;
               // m.m_maps[0].m_texture = texture;
                gs->SetMaterial(&m);

               // gs->SetRasterizerState(bqGSRasterizerState::Solid);

                while (g_run)
                {
                    bqFramework::Update();
                    

                    gs->BeginDraw();
                    gs->ClearAll();

                    gs->SetShader(bqShaderType::Line3D, 0);
                    gs->DrawLine3D(
                        bqVec4(10.f, 0.f, 0.f, 0.f), 
                        bqVec4(-10.f, 0.f, 0.f, 0.f), 
                        bqColor(1.f, 0.f, 0.f, 1.f));

                    gs->DrawLine3D(
                        bqVec4(0.f, 0.f, 10.f, 0.f),
                        bqVec4(0.f, 0.f, -10.f, 0.f),
                        bqColor(0.f, 1.f, 0.f, 1.f));

                    gs->SetShader(bqShaderType::Standart, 0);
                    
                    bqMat4 WorldViewProjection;
                    bqMat4 World;

                    static float angle = 0.f;
                    World.SetRotation(bqQuaternion(0.f, angle, 0.f));
                    angle += 0.01f;
                    if (angle > PIPI)
                        angle = 0.f;
                    WorldViewProjection = camera.m_projection * camera.m_view * World;
                    bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WorldViewProjection);
                    bqFramework::SetMatrix(bqMatrixType::World, &World);

                    for (size_t i = 0; i < model->m_gpuModels.m_size; ++i)
                    {
                        gs->SetMesh(model->m_gpuModels.m_data[i]);
                        gs->Draw();
                    }

                    gs->EndDraw();
                    gs->SwapBuffers();
                }
                delete model;
                delete texture;

                gs->Shutdown();
            }
        }

        delete window;
    }
    
    return EXIT_SUCCESS;
}
