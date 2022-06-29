//
// Created by ubuntu on 27/06/22.
//

#ifndef STAG_SOUND_READER_HRTF_H
#define STAG_SOUND_READER_HRTF_H

#include "sound_reader.h"

class SoundReaderHrtf {
public:
    SoundReaderHrtf(char* file, int size_sound_in_value);
    short* getSpatializedSound(int idx);

private:
    int isInit;
    int sampleNb;
    int angle;
    int sizeSample; // Taille d'un echantillion de la base de donnée
    short *bufferSound;
    short *emptyBuffer;
};


#endif //STAG_SOUND_READER_HRTF_H
