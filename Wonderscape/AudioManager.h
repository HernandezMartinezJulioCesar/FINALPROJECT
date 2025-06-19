#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <irrKlang/irrKlang.h>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void playBackgroundMusic();   // Reproduce música de fondo en bucle
    void stopBackgroundMusic();   // Detiene la música de fondo
    void playClickSound();        // Reproduce efecto de clic

    void setMusicVolume(float volume);   // 0.0 a 1.0
    void setEffectsVolume(float volume); // 0.0 a 1.0

private:
    irrklang::ISoundEngine* engine;
    irrklang::ISound* backgroundMusic;
    float musicVolume;
    float effectsVolume;
};

#endif
