#pragma once

namespace LiteAPI{
    class Sound;

    namespace SoundPlayer{
        void playSound(Sound* sound);
        void stopAll();
        void stopSound(Sound *sound);

        void setVolume(float volume);
        void setSpeed(float speed);
        void setPitch(float pitch);

        
    }
}