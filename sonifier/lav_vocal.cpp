//
// Created by ubuntu on 21/06/22.
//

#include "lav_vocal.h"

std::map<unsigned int, SoundReader> lavVocal::sounds;
SoundReader* lavVocal::sound = nullptr;
pthread_mutex_t lavVocal::_sound_mutex = PTHREAD_MUTEX_INITIALIZER;

void lavVocal::init()
{
    for (auto const& x : soundFile)
    {
        sounds.insert({x.first, SoundReader(x.second, SIZE_AUDIO_CHUNK_IN_VALUE)});
    }
};

void lavVocal::push_buffer(unsigned int indice)
{
    pthread_mutex_lock(&_sound_mutex);
    sound = &sounds.at(indice);
    pthread_mutex_unlock(&_sound_mutex);
}

bool lavVocal::isReading() {
    if(sound == nullptr)
        return false;
    else
        return sound->isReading();
}

void* lavVocal::pull_buffer()
{
    void * return_pointer = nullptr;
    if(sound != nullptr)
    {
        pthread_mutex_lock(&_sound_mutex);
        return_pointer = sound->pull_buffer();
        pthread_mutex_unlock(&_sound_mutex);
    }
    return return_pointer;
};
