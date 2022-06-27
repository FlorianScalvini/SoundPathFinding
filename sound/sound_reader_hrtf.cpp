//
// Created by ubuntu on 27/06/22.
//

#include "sound_reader_hrtf.h"
#include <math.h>

#define INCRE_ANGLE 5
#define NUMBER_SAMPLE 180 / INCRE_ANGLE

SoundReaderHrtf::SoundReaderHrtf() {
    sizeSample = 0;
}

int SoundReaderHrtf::init(char* file, int sizechunks) {
    if(isInit)
    {
        delete [] bufferSound;
    }
    sizeAudioChunkSample = 2*sizechunks ;
    SoundWav soundWav((char *) file);
    soundWav.readHeader();
    sizeData = soundWav.getSizeData();
    // The file %s is a monophonic sound
    if(soundWav.getNbChannel() != 2)
    {
        printf("Is not a stereophonic sound\n");
        return EXIT_FAILURE;
    }

    numberChunks = ceil((sizeData) / (sizeAudioChunkSample));
    bufferSound = new short [numberChunks *  sizeAudioChunkSample];
    memset(bufferSound, 0, numberChunks*sizeAudioChunkSample*sizeof(short));
    soundWav.readData(bufferSound, sizeData);

    sizeSample = sizeData / NUMBER_SAMPLE;
    currentChunkPtr = nullptr;
    lastChunkPtr = nullptr;
    isInit = true;
    return EXIT_SUCCESS;
}

void SoundReaderHrtf::start(unsigned int idx)
{

    if(idx > 180)
    {
        printf("%i is greater than 179\n");
        return;
    }
    int offset = round((float) idx / (float) INCRE_ANGLE);
    currentChunkPtr = bufferSound + offset*sizeSample;
    lastChunkPtr = bufferSound + offset*(sizeSample + 1) - sizeAudioChunkSample;;
}

