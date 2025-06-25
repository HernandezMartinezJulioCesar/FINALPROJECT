#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <irrKlang/irrKlang.h>
#include <iostream>

class AudioManager {
public:
    AudioManager(); // Initializes irrKlang's audio engine and sets default volumes
    ~AudioManager(); // Frees up audio resources and stops any sound that is playing

    void playBackgroundMusic(std::string music);
    void stopBackgroundMusic();
    void playClickSound(std::string sound);

    void setMusicVolume(float volume);
    void setEffectsVolume(float volume);

private:
    irrklang::ISoundEngine* engine; // irrKlang audio engine
    irrklang::ISound* backgroundMusic; // Pointer to current background music
    // Values (0.0f - 1.0f)
    float musicVolume;
    float effectsVolume;
};

#endif
