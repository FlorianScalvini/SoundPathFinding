//
// Created by ubuntu on 21/06/22.
//

#ifndef STAG_LAV_VOCAL_H
#define STAG_LAV_VOCAL_H

#include "../sound/sound_reader.h"
#include "lav_constants.h"
#include <vector>
#include <map>

std::map<unsigned int, const char* > soundFile =     {
        { 0, "/home/ubuntu/Bureau/HrtfConvo/person.wav" },
        { 1, "/home/ubuntu/Bureau/HrtfConvo/person.wav" },
};



class lavVocal {
public:
    static void init();
    static void* pull_buffer();
    static void push_buffer(unsigned int indice);
    static bool isReading();

    static pthread_mutex_t _sound_mutex;
    static std::map<unsigned int, SoundReader> sounds;
    static SoundReader* sound;
};


#endif //STAG_LAV_VOCAL_H
