//
// Created by ubuntu on 21/06/22.
//

#include "lav_vocal.h"



std::vector<SoundReader> lavVocal::sounds;
SoundReader* lavVocal::sound = nullptr;
pthread_mutex_t lavVocal::_sound_mutex = PTHREAD_MUTEX_INITIALIZER;
short* lavVocal::emptyBuffer;
int lavVocal::idx;
void lavVocal::init()
{
    sounds = std::vector<SoundReader>();
    sounds.clear();
    std::vector<char* > soundFile =     {
            { "/home/ubuntu/CLionProjects/pathFinder/person44100.wav" },
            { "/home/ubuntu/CLionProjects/pathFinder/person44100.wav" },
            { "/home/ubuntu/CLionProjects/pathFinder/person44100.wav" },
            { "/home/ubuntu/CLionProjects/pathFinder/person44100.wav" },
            { "/home/ubuntu/CLionProjects/pathFinder/person44100.wav" },
            { "/home/ubuntu/CLionProjects/pathFinder/person44100.wav" }
    };
    for(int i = 0; i < soundFile.size(); i++)
    {
        SoundReader newSound = SoundReader();
        newSound.init(soundFile[0], SIZE_AUDIO_CHUNK_IN_SAMPLE);
        sounds.emplace_back(newSound);
    }

    idx = -1;
    emptyBuffer = new short[SIZE_AUDIO_CHUNK_IN_VALUE];
    sound = nullptr;
};

void lavVocal::start(unsigned int indice)
{
    idx = indice;
    pthread_mutex_lock(&_sound_mutex);
    sound = &sounds[indice];
    sound->start();
    pthread_mutex_unlock(&_sound_mutex);
}

bool lavVocal::isReading() {
    if(sound == nullptr)
        return false;
    else
    if(!sound->isReading())
    {
        sound == nullptr;
        return false;
    }
    else
        return true;
}

void* lavVocal::pull_buffer()
{
    void * return_pointer = emptyBuffer;
    if(sound != nullptr)
    {
        pthread_mutex_lock(&_sound_mutex);
        return_pointer = sound->pull_buffer();
        pthread_mutex_unlock(&_sound_mutex);
    }
    return return_pointer;
};

