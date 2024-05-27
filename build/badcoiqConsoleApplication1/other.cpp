﻿#include "badcoiq/system/bqWindow.h"
#include "badcoiq/input/bqInput.h"
#include "badcoiq/gs/bqGS.h"
#include "badcoiq/math/bqMath.h"
#include "badcoiq/containers/bqList.h"
#include "badcoiq/containers/bqArray.h"

#include "badcoiq/geometry/bqMeshLoader.h"
#include "badcoiq/archive/bqArchive.h"
#include "badcoiq/sound/bqSoundSystem.h"

#include "badcoiq/GUI/bqGUI.h"

#include "badcoiq/scene/bqCamera.h"
#include <list>

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
        g_run = false;
    }

    virtual void OnSize(bqWindow* w) 
    {
        bqGS* gs = (bqGS*)GetUserData();
        if(gs)
            gs->OnWindowSize();
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

#ifdef BQ_WITH_GUI
class MyGUIDrawTextCallback : public bqGUIDrawTextCallback
{
    bqColor m_colorBlack;
    bqGUIFont* m_defaultFont = 0;
public:
    MyGUIDrawTextCallback() {}
    virtual ~MyGUIDrawTextCallback() {}

    virtual bqGUIFont* OnFont(uint32_t, char32_t)
    {
        return m_defaultFont;
    }

    virtual bqColor* OnColor(uint32_t, char32_t)
    {
        return &m_colorBlack;
    }

    void SetFont(bqGUIFont* f)
    {
        m_defaultFont = f;
    }
};

class MyButton : public bqGUIButton
{
public:
    MyButton(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
        :
        bqGUIButton(w, position, size)
    {
    }

    virtual ~MyButton()
    {
    }

    BQ_PLACEMENT_ALLOCATOR(MyButton);
  
    virtual void OnClickLMB() override
    {
        bqLog::Print("BTN LMB\n");
        SetText(U"BTN");
    }
};

class MyCheckBox : public bqGUICheckRadioBox
{
public:
    MyCheckBox(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
        :
        bqGUICheckRadioBox(w, position, size)
    {
    }

    virtual ~MyCheckBox() {}
};

class MyRadiobutton : public bqGUICheckRadioBox
{
public:
    MyRadiobutton(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
        :
        bqGUICheckRadioBox(w, position, size)
    {
        m_asRadioButton = true;
        m_radiouGroup = 1;
    }

    virtual ~MyRadiobutton() {}
};

class MyTextEditor : public bqGUITextEditor
{
public:
    MyTextEditor(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size):
        bqGUITextEditor(w, position, size)
    {}
    virtual ~MyTextEditor() {}
};

class MyListBox : public bqGUIListBox
{
public:
    MyListBox(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size) :
        bqGUIListBox(w, position, size)
    {}
    virtual ~MyListBox() {}
};

class MySlider : public bqGUISlider
{
public:
    MySlider(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size) :
        bqGUISlider(w, position, size)
    {}
    virtual ~MySlider() {}
};

class TestGUIScrollbar : public bqGUIScrollbar
{
public:
    TestGUIScrollbar(bqGUIWindow* w, const bqVec2f& position, const bqVec2f& size)
        :
        bqGUIScrollbar(w, position, size)
    {}
    virtual ~TestGUIScrollbar() {}
    BQ_PLACEMENT_ALLOCATOR(TestGUIScrollbar);
};
#endif

class MySoundMixerCallback : public bqSoundMixerCallback
{
    bqSound* m_sound = 0;
public:
    MySoundMixerCallback() 
    {
        m_sound = new bqSound(1, 41000, bqSoundFormat::float32);
        //m_sound->Convert(bqSoundFormat::float32);
    }

    virtual ~MySoundMixerCallback() 
    {
        if (m_sound) delete m_sound;
    }

    virtual void OnStartProcess(bqSoundMixer*)
    {
    //    bqLog::PrintInfo("%s\n", BQ_FUNCTION);
    }

    // Когда завершается смешивание звука в общую кучу.
    virtual void OnEndMixSound(bqSoundMixer* mixer, bqSound*)
    {
      //  bqLog::PrintInfo("%s\n", BQ_FUNCTION);

        auto effects = mixer->GetEffects();

        static float angle = 0.0f;
        float s = sinf(angle);

        auto curr = effects->m_head;
        auto end = effects->m_head->m_left;
        while (true)
        {
            bqSoundEffectVolume* effect = dynamic_cast<bqSoundEffectVolume*>(curr->m_data);
            if (effect)
            {
    //            effect->SetVolume(s);
            }

            curr = curr->m_right;
            if (curr == end)
                break;
        }

        angle += 0.0005f;
        if (angle > PIf)
            angle = 0.f;
    }

    // Когда прошлись по всем звукам.
    virtual void OnEndProcess(bqSoundMixer* mixer)
    {
     //   bqLog::PrintInfo("%s\n", BQ_FUNCTION);

        if (mixer)
        {
            if (mixer->GetNumOfChannels())
            {
                bqSoundBufferInfo inf;
                mixer->GetSoundBufferInfo(inf);

                m_sound->m_soundBuffer->Resample(bqFramework::GetSoundSystem()->GetDeviceInfo().m_sampleRate);

                m_sound->m_soundBuffer->Append(mixer->GetChannel(0), &inf);
            }

            if (!m_numOfSounds)
                m_sound->m_soundBuffer->SaveToFile(bqSoundFileType::wav, "test_aftereffect.wav");
        }
    }

    // Когда звук обработан полностью.
    virtual void OnEndSound(bqSoundMixer* mixer, bqSound* s)
    {
        bqLog::PrintInfo("%s\n", BQ_FUNCTION);

        //m_isFinished = true;
        // 
        if(m_numOfSounds)
            --m_numOfSounds;

        
    }

    virtual void OnFullBuffer(bqSoundMixer* mixer)
    {
        //   bqLog::PrintInfo("%s\n", BQ_FUNCTION);

       
    }

    //bool m_isFinished = false;
    int m_numOfSounds = 0;
};

#include "badcoiq/containers/bqFixedFIFO.h"

int main()
{
    bqFrameworkCallbackCB fcb;
    bqFramework::Start(&fcb);

    MySoundMixerCallback mixerCallback;

    bqWindowCallbackCB wcb;
    auto window = bqFramework::SummonWindow(&wcb);
    if (window && bqFramework::GetGSNum())
    {
        window->SetPositionAndSize(10, 10, 800, 600);
        window->SetVisible(true);

        bqArchiveSystem::ZipAdd("../data/data.zip");

        bqGS* gs = bqFramework::SummonGS(bqFramework::GetGSUID(0));
        if (gs)
        {
            wcb.SetUserData(gs);

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
                bqMat4 ViewProjection = camera.GetMatrixProjection() * camera.GetMatrixView();
                bqFramework::SetMatrix(bqMatrixType::ViewProjection, &ViewProjection);
                
                MyModel* model = new MyModel(gs);
                model->Load(bqFramework::GetPath("../data/models/1.obj").c_str());
                
                bqImage* image = bqFramework::SummonImage(bqFramework::GetPath("../data/image.bmp").c_str());
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

#ifdef BQ_WITH_GUI
                bqFramework::InitDefaultFonts(gs);
                MyGUIDrawTextCallback tdcb;
                tdcb.SetFont(bqFramework::GetDefaultFont(bqGUIDefaultFont::Text));

                auto guiWindow = bqFramework::SummonGUIWindow(window, bqVec2f(100.f, 100.f), bqVec2f(300.f));
                guiWindow->m_windowFlags |= bqGUIWindow::windowFlag_withTitleBar;
                guiWindow->m_windowFlags |= bqGUIWindow::windowFlag_canMove;
                guiWindow->m_windowFlags |= bqGUIWindow::windowFlag_canResize;
           //     guiWindow->m_windowFlags |= bqGUIWindow::windowFlag_disableScrollbar;
                MyButton* btn = new MyButton(guiWindow, bqVec2f(0.f, 0.f), bqVec2f(50.f, 420.f));
                MyCheckBox* chckbx = new MyCheckBox(guiWindow, bqVec2f(60.f, 0.f), bqVec2f(100.f, 20.f));
                chckbx->SetText(U"Use thing");
                MyRadiobutton* rdbtn1 = new MyRadiobutton(guiWindow, bqVec2f(60.f, 20.f), bqVec2f(100.f, 20.f));
                MyRadiobutton* rdbtn2 = new MyRadiobutton(guiWindow, bqVec2f(60.f, 40.f), bqVec2f(100.f, 20.f));
                MyRadiobutton* rdbtn3 = new MyRadiobutton(guiWindow, bqVec2f(60.f, 60.f), bqVec2f(100.f, 20.f));
                MyRadiobutton* rdbtn4 = new MyRadiobutton(guiWindow, bqVec2f(60.f, 80.f), bqVec2f(100.f, 20.f));
                MyRadiobutton* rdbtn5 = new MyRadiobutton(guiWindow, bqVec2f(60.f, 100.f), bqVec2f(100.f, 20.f));
                MyRadiobutton* rdbtn6 = new MyRadiobutton(guiWindow, bqVec2f(60.f, 120.f), bqVec2f(100.f, 20.f));
                rdbtn4->m_radiouGroup = 2;
                rdbtn5->m_radiouGroup = 2;
                rdbtn6->m_radiouGroup = 2;
                rdbtn1->SetText(U"Radio1 g1");
                rdbtn2->SetText(U"Radio2 g1");
                rdbtn3->SetText(U"Radio3 g1");
                rdbtn4->SetText(U"Radio4 g2");
                rdbtn5->SetText(U"Radio5 g2");
                rdbtn6->SetText(U"Radio6 g2");
                rdbtn1->m_isChecked = true;
                rdbtn5->m_isChecked = true;
                MyTextEditor* txtedtr = new MyTextEditor(guiWindow, bqVec2f(60.f, 140.f), bqVec2f(150.f, 200.f));
                MyListBox* lstbx = new MyListBox(guiWindow, bqVec2f(180.f, 20.f), bqVec2f(60.f, 70.f));
                lstbx->AddItem(U"Item1", 0, 0);
                lstbx->AddItem(U"Item12", 0, 0);
                lstbx->AddItem(U"Item13", 0, 0);
                lstbx->AddItem(U"Item14", 0, 0);
                lstbx->AddItem(U"Item15", 0, 0);
                lstbx->AddItem(U"Item16", 0, 0);
                lstbx->AddItem(U"Item17", 0, 0);
                lstbx->AddItem(U"Item18", 0, 0);
                lstbx->AddItem(U"Item19", 0, 0);
                lstbx->AddItem(U"Item10", 0, 0);
                lstbx->AddItem(U"Itemq1", 0, 0);
                lstbx->AddItem(U"Itemw1", 0, 0);
                lstbx->AddItem(U"Iteme1", 0, 0);
                lstbx->AddItem(U"Item1tr", 0, 0);
                lstbx->AddItem(U"Item1y", 0, 0);
                lstbx->AddItem(U"Item1i", 0, 0);
                lstbx->AddItem(U"Item1o", 0, 0);
                lstbx->AddItem(U"Item1p", 0, 0);
                lstbx->AddItem(U"Item1k", 0, 0);
                MySlider* sld = new MySlider(guiWindow, bqVec2f(180.f, 100.f), bqVec2f(90.f, 30.f));
                sld->m_morePrecise = true;
              /*  TestGUIScrollbar* testScrollbar = new TestGUIScrollbar(guiWindow, bqVec2f(), bqVec2f(10.f, 40.f));
                testScrollbar->m_alignment = bqGUIElement::Alignment::Left;*/
                sld->m_alignment = bqGUIElement::Alignment::Center;
                bqFramework::RebuildGUI();
#endif

                bqSound sound1;
                bqSound sound2;
                bqSound sound3;
                bqSound sound4;
                bqSound sound5;
                bqSound sound6;
                bqSound sound7;
                bqSound sound7b;
                bqSound sound8;
                uint32_t Hz = 440;

                float time = 0.5f;
                float loudness = 0.5f;
                sound1.m_soundBuffer = new bqSoundBuffer;
                sound1.m_hasItsOwnSound = true;
                sound2.m_soundBuffer = new bqSoundBuffer;
                sound2.m_hasItsOwnSound = true;
                sound3.m_soundBuffer = new bqSoundBuffer;
                sound3.m_hasItsOwnSound = true;
                sound4.m_soundBuffer = new bqSoundBuffer;
                sound4.m_hasItsOwnSound = true;
                sound5.m_soundBuffer = new bqSoundBuffer;
                sound5.m_hasItsOwnSound = true;
                sound6.m_soundBuffer = new bqSoundBuffer;
                sound6.m_hasItsOwnSound = true;
                
                sound7.m_soundBuffer = new bqSoundBuffer;
                sound7.m_hasItsOwnSound = true;
                sound7b.m_soundBuffer = sound7.m_soundBuffer;

                sound8.m_soundBuffer = new bqSoundBuffer;
                sound8.m_hasItsOwnSound = true;

                sound1.m_soundBuffer->Generate(bqSoundWaveType::sin, time, Hz, loudness);
                sound2.m_soundBuffer->Generate(bqSoundWaveType::square, time, Hz, loudness);
                sound3.m_soundBuffer->Generate(bqSoundWaveType::triangle, time, Hz, loudness);
                sound4.m_soundBuffer->Generate(bqSoundWaveType::saw, time, Hz, loudness);
                sound5.m_soundBuffer->Create(1.f, 2, 44100, bqSoundFormat::int16);

              //  sound2.SaveToFile(bqSoundFileType::wav, "square.wav");
              //  sound3.SaveToFile(bqSoundFileType::wav, "triangle.wav");
              //  sound4.SaveToFile(bqSoundFileType::wav, "saw.wav");
              //  sound5.SaveToFile(bqSoundFileType::wav, "2channels.wav");

                sound6.m_soundBuffer->LoadFromFile("../data/sounds/alien_beacon44100_float.wav");
                sound6.m_soundBuffer->Resample(100000);
                sound6.m_soundBuffer->SaveToFile(bqSoundFileType::wav, "../data/sounds/alien_beacon44100_float_resample_100000.wav");

                auto soundSystem = bqFramework::GetSoundSystem();
                auto soundDeviceInfo = soundSystem->GetDeviceInfo();
                sound7.m_soundBuffer->LoadFromFile("../data/sounds/Clearlight.wav");
                sound7.m_soundBuffer->Make32bitsFloat();
              //  sound7.m_soundBuffer->MakeMono(0);
                sound7.m_soundBuffer->Resample(soundDeviceInfo.m_sampleRate);

                sound8.m_soundBuffer->LoadFromFile("../data/sounds/flute.wav");
                sound8.m_soundBuffer->Make32bitsFloat();
             //   sound8.m_soundBuffer->MakeMono(0);
                sound8.m_soundBuffer->Resample(soundDeviceInfo.m_sampleRate);
                
                bqSoundEffectVolume* sfx_volume = 0;
                auto mixer = soundSystem->SummonMixer(2);
                if (mixer)
                {
                    //mixer->SetCallback(&mixerCallback);
                    //sound7.SetLoop(1);
                    mixer->AddSound(&sound7);
                    mixer->AddSound(&sound7b);
                   // mixer->AddSound(&sound8);
                    mixerCallback.m_numOfSounds = 2;

                    sfx_volume = soundSystem->SummonEffectVolume();
                    sfx_volume->SetVolume(0.1f);
                    mixer->AddEffect(sfx_volume);
                    
                    for (int i = 0; i < 3000; ++i)
                    {
                        mixer->Process();
                    }
                    mixerCallback.m_numOfSounds = 0;
                    mixer->Process();

                    soundSystem->AddMixerToProcessing(mixer);
                    mixer->UseProcessing(true);
                }
              //  bqFramework::GetSoundSystem()->
                
                {
                //    BQ_PTR_D(bqSoundObject, so1, se->SummonSoundObject(&sound1));
                //    BQ_PTR_D(bqSoundObject, so2, se->SummonSoundObject(&sound2));
                //    BQ_PTR_D(bqSoundObject, so3, se->SummonSoundObject(&sound3));
               //     BQ_PTR_D(bqSoundObject, so4, se->SummonSoundObject(&sound4));
               //     BQ_PTR_D(bqSoundObject, so5, se->SummonSoundObject(&sound6));

                    float* dt_ptr = bqFramework::GetDeltaTime();
                    while (g_run)
                    {
                        bqFramework::Update();
#ifdef BQ_WITH_GUI
                        bqFramework::UpdateGUI();
#endif

                        if (bqInput::IsKeyHit(bqInput::KEY_1))
                        {
                            float volume = mixer->GetVolume();
                            volume -= 1.f * (*dt_ptr);
                            if (volume < 0.f)
                                volume = 0.f;
                            printf("V: %f\n", volume);
                            mixer->SetVolume(volume);
                        }
                        if (bqInput::IsKeyHit(bqInput::KEY_2))
                        {
                            float volume = mixer->GetVolume();
                            volume += 1.f * (*dt_ptr);
                            if (volume > 1.f)
                                volume = 1.f;
                            printf("V: %f\n", volume);
                            mixer->SetVolume(volume);
                        }

                        if (bqInput::IsKeyHit(bqInput::KEY_3))
                        {
                            sound7.m_pitch -= 0.01f;
                            printf("Pitch: %f\n", sound7.m_pitch);
                        }
                        if (bqInput::IsKeyHit(bqInput::KEY_4))
                        {
                            sound7.m_pitch += 0.01f;
                            printf("Pitch: %f\n", sound7.m_pitch);
                        }
                        if (bqInput::IsKeyHit(bqInput::KEY_5))
                        {
                            sound7.m_pitchLimitDown -= 0.01f;
                            printf("PitchLimitD: %f\n", sound7.m_pitchLimitDown);
                        }
                        if (bqInput::IsKeyHit(bqInput::KEY_6))
                        {
                            sound7.m_pitchLimitDown += 0.01f;
                            printf("PitchLimitD: %f\n", sound7.m_pitchLimitDown);
                        }
                        if (bqInput::IsKeyHit(bqInput::KEY_7))
                        {
                            sound7.m_pitchLimitUp -= 0.01f;
                            printf("PitchLimitU: %f\n", sound7.m_pitchLimitUp);
                        }
                        if (bqInput::IsKeyHit(bqInput::KEY_8))
                        {
                            sound7.m_pitchLimitUp += 0.01f;
                            printf("PitchLimitU: %f\n", sound7.m_pitchLimitUp);
                        }
                        if (bqInput::IsKeyHit(bqInput::KEY_9))
                        {
                            sound7.m_pitch = 1.0f;
                            sound7.m_pitchLimitUp = 3.0f;
                            sound7.m_pitchLimitDown = 3.0f;
                        }

                        if (bqInput::IsKeyHit(bqInput::KEY_Q))
                        {
                            sound7.PlaybackStart();
                        }
                        if (bqInput::IsKeyHit(bqInput::KEY_W))
                        {
                            sound7.PlaybackSet(17.5f);
                        }
                        if (bqInput::IsKeyHit(bqInput::KEY_E))
                        {
                            sound7.PlaybackStop();
                            sound7b.PlaybackStop();
                        }
                        if (bqInput::IsKeyHit(bqInput::KEY_R))
                        {
                            sound7.SetRegion(0.f, 0.5f);
                        }
                        if (bqInput::IsKeyHit(bqInput::KEY_T))
                        {
                            sound7.SetRegion(0.f, 0.f);
                        }

                       /* if (bqInput::IsKeyHit(bqInput::KEY_1))
                            so1->Start();
                        if (bqInput::IsKeyHit(bqInput::KEY_2))
                            so2->Start();
                        if (bqInput::IsKeyHit(bqInput::KEY_3))
                            so3->Start();
                        if (bqInput::IsKeyHit(bqInput::KEY_4))
                            so4->Start();
                        if (bqInput::IsKeyHit(bqInput::KEY_5))
                            so5->Start();
                        if (bqInput::IsKeyHit(bqInput::KEY_6))
                            so5->Stop();*/

                        if (bqInput::IsKeyHit(bqInput::KEY_PGDOWN))
                        {
                            --Hz;
                            if (Hz < 5)
                                Hz = 5;
                            printf("Hz: %u\n", Hz);
                            sound1.m_soundBuffer->Generate(bqSoundWaveType::sin, 1.f, Hz);
                            sound2.m_soundBuffer->Generate(bqSoundWaveType::square, 1.f, Hz);
                            sound3.m_soundBuffer->Generate(bqSoundWaveType::triangle, 1.f, Hz);
                            sound4.m_soundBuffer->Generate(bqSoundWaveType::saw, 1.f, Hz);
                        }

                        if (bqInput::IsKeyHit(bqInput::KEY_PGUP))
                        {
                            ++Hz;
                            if (Hz > 20000)
                                Hz = 20000;
                            printf("Hz: %u\n", Hz);
                            sound1.m_soundBuffer->Generate(bqSoundWaveType::sin, 1.f, Hz);
                            sound2.m_soundBuffer->Generate(bqSoundWaveType::square, 1.f, Hz);
                            sound3.m_soundBuffer->Generate(bqSoundWaveType::triangle, 1.f, Hz);
                            sound4.m_soundBuffer->Generate(bqSoundWaveType::saw, 1.f, Hz);
                        }

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
                        WorldViewProjection = camera.GetMatrixProjection() * camera.GetMatrixView() * World;
                        bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WorldViewProjection);
                        bqFramework::SetMatrix(bqMatrixType::World, &World);

                        for (size_t i = 0; i < model->m_gpuModels.m_size; ++i)
                        {
                            gs->SetMesh(model->m_gpuModels.m_data[i]);
                            gs->Draw();
                        }

#ifdef BQ_WITH_GUI
                        gs->BeginGUI();
                        gs->DrawGUIRectangle(bqVec4f(0.f, 0.f, 100.f, 20.f), bq::ColorRed, bq::ColorYellow, 0, 0);
                        gs->DrawGUIText(U"Hello!!!", 9, bqVec2f(10.f), &tdcb);
                        bqFramework::DrawGUI(gs);
                        gs->EndGUI();
#endif

                        gs->EndDraw();
                        gs->SwapBuffers();
                    }
                }
                delete model;
                delete texture;

                soundSystem->RemoveAllMixersFromProcessing();

                if (sfx_volume)
                   delete sfx_volume;

                if (mixer) delete mixer;

                gs->Shutdown();
            }
        }

        delete window;
    }
    
    return EXIT_SUCCESS;
}
