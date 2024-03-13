// Класс который смешивает звуки в одно целое.
// Должны быть реализованы эффекты.
// Звуки должны добавляться, возможно в специальную оболочку, так как надо запоминать текущую позицию воспроизведения.

class bqSoundMixer
{
public:

bqSoundMixer(){}
virtual ~bqSoundMixer(){}

virtual void SetVolume(float) = 0;
virtual float GetVolume() = 0;

virtual void AddSound(bqSound*) = 0;
virtual void RemoveSound(bqSound*) = 0;
virtual void RemoveAllSounds() = 0;
};


