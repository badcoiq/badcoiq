#include "badcoiq.h"
#include "badcoiq/system/bqWindow.h"
#include "badcoiq/input/bqInput.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/math/bqMath.h"
#include "badcoiq/containers/bqList.h"

#include "badcoiq/geometry/bqMeshCreator.h"

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

        bqGS* gs = bqFramework::SummonGS(bqFramework::GetGSUID(0));
        if (gs)
        {
            if (gs->Init(window, 0))
            {
                gs->SetClearColor(0.3f, 0.3f, 0.3f, 1.f);

                bqCamera camera;
                camera.m_position.Set(2.f, 1.f, 0.0f, 0.f);
                camera.m_aspect = 300.f / 200.f;
                camera.m_fov = 1.1;
                
                camera.SetType(bqCamera::Type::PerspectiveLookAt);

                camera.m_rotation.Set(bqMath::DegToRad(-75.f), 0.0, 0.f);
                camera.Update(*bqFramework::GetDeltaTime());
                
               // bqMath::LookAtRH(camera.m_view, camera.m_position, bqVec4(), bqVec4(0.f, 1.f, 0.f, 0.f));
                bqMat4 ViewProjection = camera.m_projection * camera.m_view;
                bqFramework::SetMatrix(bqMatrixType::ViewProjection, &ViewProjection);
                
                float H = 1.1f;
                bqPolygonMesh pm;
                bqMeshPolygonCreator pc;
                pc.SetPosition(bqVec3f(-1.f, 0.f, 1.f));
                pc.AddVertex();
                pc.SetPosition(bqVec3f(0.f, H, 0.f));
                pc.AddVertex();
                pc.SetPosition(bqVec3f(1.f, 0.f, 1.f));
                pc.AddVertex();
                pm.AddPolygon(&pc, true);
                pc.Clear();

                pc.SetPosition(bqVec3f(0.f, H, 0.f));
                pc.AddVertex();
                pc.SetPosition(bqVec3f(0.f, 0.f, -1.f));
                pc.AddVertex();
                pc.SetPosition(bqVec3f(1.f, 0.f, 1.f));
                pc.AddVertex();
                pm.AddPolygon(&pc, true);
                pc.Clear();

                pc.SetPosition(bqVec3f(0.f, 0.f, -1.f));
                pc.AddVertex();
                pc.SetPosition(bqVec3f(0.f, H, 0.f));
                pc.AddVertex();
                pc.SetPosition(bqVec3f(-1.f, 0.f, 1.f));
                pc.AddVertex();
                pm.AddPolygon(&pc, true);
                pc.Clear();

                pm.GenerateNormals(false);
                
                bqMesh* mesh = pm.SummonMesh();
                bqGPUMesh* gpuMesh = gs->SummonMesh(mesh);
                delete mesh;

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
                    gs->SetMesh(gpuMesh);

                    bqMaterial m;
                    m.m_sunPosition.Set(1.f, 3.f, -1.f);
                    m.m_sunPosition.Normalize();
                    m.m_shaderType = bqShaderType::Standart;
                    gs->SetMaterial(&m);

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
                    gs->Draw();

                    gs->EndDraw();
                    gs->SwapBuffers();
                }
                
                delete gpuMesh;

                gs->Shutdown();
            }
        }

        delete window;
    }
    
    return EXIT_SUCCESS;
}
