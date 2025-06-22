#include "AudioManager.h"

AudioManager::AudioManager()
    : backgroundMusic(nullptr), musicVolume(0.4f), effectsVolume(0.6f)
{
    engine = irrklang::createIrrKlangDevice();
}

AudioManager::~AudioManager() {
    if (backgroundMusic) {
        backgroundMusic->stop();
        backgroundMusic->drop();
    }
    if (engine)
        engine->drop();
}

void AudioManager::playBackgroundMusic(std::string music) {
    if (!engine) return;

    if (!backgroundMusic) {
        backgroundMusic = engine->play2D(music.c_str(), true, false, true);
        if (backgroundMusic)
            backgroundMusic->setVolume(musicVolume);
    }
}

void AudioManager::stopBackgroundMusic() {
    if (backgroundMusic) {
        backgroundMusic->stop();
        backgroundMusic->drop();
        backgroundMusic = nullptr;
    }
}

void AudioManager::playClickSound(std::string sound) {
    if (!engine) return;

    irrklang::ISound* clickSound = engine->play2D(sound.c_str(), false, false, true);
    if (clickSound)
        clickSound->setVolume(effectsVolume);
}

void AudioManager::setMusicVolume(float volume) {
    musicVolume = volume;
    if (backgroundMusic)
        backgroundMusic->setVolume(musicVolume);
}

void AudioManager::setEffectsVolume(float volume) {
    effectsVolume = volume;
}
