
#include "badcoiq/input/bqInput.h"
#include "badcoiq/sound/bqSoundSystem.h"

class bqFrameworkCallbackCB : public bqFrameworkCallback
{
public:
    bqFrameworkCallbackCB() {}
    virtual ~bqFrameworkCallbackCB() {}
    virtual void OnMessage() {}
};

bool g_run = true;

int main()
{
    bqFrameworkCallbackCB fcb;
    bqFramework::Start(&fcb);

    bqSound sound;
    sound.m_soundBuffer = new bqSoundBuffer;
    sound.m_hasItsOwnSound = true;

    auto soundSystem = bqFramework::GetSoundSystem();
    auto soundDeviceInfo = soundSystem->GetDeviceInfo();
    sound.m_soundBuffer->LoadFromFile("../data/sounds/Clearlight.wav");
    sound.m_soundBuffer->Make32bitsFloat();
    //  sound.m_soundBuffer->MakeMono(0);
    sound.m_soundBuffer->Resample(soundDeviceInfo.m_sampleRate);

    auto mixer = soundSystem->SummonMixer(2);
    if (mixer)
    {
        mixer->AddSound(&sound);
        soundSystem->AddMixerToProcessing(mixer);
    }

    float* dt_ptr = bqFramework::GetDeltaTime();
    while (g_run)
    {
        bqFramework::Update();
        
        float a = 1.f * (*dt_ptr);;

        if (bqInput::IsKeyHit(bqInput::KEY_1))
        {
            float volume = mixer->GetVolume();
            volume -= a;
            if (volume < 0.f)
                volume = 0.f;
            printf("V: %f\n", volume);
            mixer->SetVolume(volume);
        }
        if (bqInput::IsKeyHit(bqInput::KEY_2))
        {
            float volume = mixer->GetVolume();
            volume += a;
            if (volume > 1.f)
                volume = 1.f;
            printf("V: %f\n", volume);
            mixer->SetVolume(volume);
        }

        if (bqInput::IsKeyHit(bqInput::KEY_3))
        {
            sound.m_pitch -= a;
            printf("Pitch: %f\n", sound.m_pitch);
        }
        if (bqInput::IsKeyHit(bqInput::KEY_4))
        {
            sound.m_pitch += a;
            printf("Pitch: %f\n", sound.m_pitch);
        }
        if (bqInput::IsKeyHit(bqInput::KEY_5))
        {
            sound.m_pitchLimitDown -= a;
            printf("PitchLimitD: %f\n", sound.m_pitchLimitDown);
        }
        if (bqInput::IsKeyHit(bqInput::KEY_6))
        {
            sound.m_pitchLimitDown += a;
            printf("PitchLimitD: %f\n", sound.m_pitchLimitDown);
        }
        if (bqInput::IsKeyHit(bqInput::KEY_7))
        {
            sound.m_pitchLimitUp -= a;
            printf("PitchLimitU: %f\n", sound.m_pitchLimitUp);
        }
        if (bqInput::IsKeyHit(bqInput::KEY_8))
        {
            sound.m_pitchLimitUp += a;
            printf("PitchLimitU: %f\n", sound.m_pitchLimitUp);
        }
        if (bqInput::IsKeyHit(bqInput::KEY_9))
        {
            sound.m_pitch = 1.0f;
            sound.m_pitchLimitUp = 3.0f;
            sound.m_pitchLimitDown = 3.0f;
        }

        if (bqInput::IsKeyHit(bqInput::KEY_Q))
        {
            sound.PlaybackStart();
        }
        if (bqInput::IsKeyHit(bqInput::KEY_W))
        {
            sound.PlaybackSet(17.5f);
        }
        if (bqInput::IsKeyHit(bqInput::KEY_E))
        {
            sound.PlaybackStop();
        }
        if (bqInput::IsKeyHit(bqInput::KEY_R))
        {
            sound.SetRegion(0.f, 0.5f);
        }
        if (bqInput::IsKeyHit(bqInput::KEY_T))
        {
            sound.SetRegion(0.f, 0.f);
        }
    }

    soundSystem->RemoveAllMixersFromProcessing();
    if (mixer) 
        delete mixer;

    return EXIT_SUCCESS;
}
