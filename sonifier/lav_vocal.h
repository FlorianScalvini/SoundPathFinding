//
// Created by ubuntu on 21/06/22.
//

#ifndef STAG_LAV_VOCAL_H
#define STAG_LAV_VOCAL_H

#include "../sound/sound_reader.h"
#include "lav_constants.h"
#include <vector>
#include <map>




class lavVocal {
public:
    static void init();
    static void* pull_buffer();
    static void push_buffer(unsigned int indice);
    static bool isReading();
    static short* emptyBuffer;

    static int idx;
    static pthread_mutex_t _sound_mutex;
    static std::vector<SoundReader> sounds;
    static SoundReader* sound;
};


#endif //STAG_LAV_VOCAL_H
