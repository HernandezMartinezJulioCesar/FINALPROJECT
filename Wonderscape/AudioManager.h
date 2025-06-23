#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <irrKlang/irrKlang.h>
#include <iostream>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void playBackgroundMusic(std::string music);   // Reproduce background music in a loop
    void stopBackgroundMusic();   // Stop the background music
    void playClickSound(std::string sound);        // Play an audio

    void setMusicVolume(float volume);   // 0.0 a 1.0
    void setEffectsVolume(float volume); // 0.0 a 1.0

private:
    irrklang::ISoundEngine* engine;
    irrklang::ISound* backgroundMusic;
    float musicVolume;
    float effectsVolume;
};

#endif
