//
// Created by ubuntu on 21/06/22.
//

#include "lav_vocal.h"

std::vector<const char* > soundFile = {
        { "/home/ubuntu/CLionProjects/pathFinder/person44100.wav" },
        { "/home/ubuntu/CLionProjects/pathFinder/person44100.wav" },
        { "/home/ubuntu/CLionProjects/pathFinder/person44100.wav" },
        { "/home/ubuntu/CLionProjects/pathFinder/person44100.wav" },
        { "/home/ubuntu/CLionProjects/pathFinder/person44100.wav" },
        { "/home/ubuntu/CLionProjects/pathFinder/person44100.wav" }
};

std::vector<SoundReader> lavVocal::sounds;
SoundReader* lavVocal::ptrSound = nullptr;
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
    ptrSound = nullptr;
};

void lavVocal::start(unsigned int indice)
{
    idx = indice;
    pthread_mutex_lock(&_sound_mutex);
    ptrSound = &sounds[indice];
    ptrSound->start();
    pthread_mutex_unlock(&_sound_mutex);
}

bool lavVocal::isReading() {
    if(ptrSound == nullptr)
        return false;
    else
    if(!ptrSound->isReading())
    {
        pthread_mutex_lock(&_sound_mutex);
        ptrSound == nullptr;
        pthread_mutex_unlock(&_sound_mutex);
        return false;
    }
    else
        return true;
}

void* lavVocal::pull_buffer()
{
    void * return_pointer = emptyBuffer;
    pthread_mutex_lock(&_sound_mutex);
    if(ptrSound != nullptr)
    {
        return_pointer = ptrSound->pull_buffer();
    }
    pthread_mutex_unlock(&_sound_mutex);
    return return_pointer;
};

