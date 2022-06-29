//
// Created by ubuntu on 27/06/22.
//

#include "sound_reader_hrtf.h"
#include <math.h>

#define INCRE_ANGLE 5
#define NUMBER_SAMPLE 37

SoundReaderHrtf::SoundReaderHrtf(char  * file, int size_sound_in_value) {

    SoundWav soundWav((char *) file);
    soundWav.readHeader();
    int sizeData = soundWav.getSizeData();
    isInit = false;
    // The file %s is a monophonic sound
    if(soundWav.getNbChannel() != 2 and sizeData % 512 == 0)
    {
        printf("Is not a stereophonic sound\n");
        return;
    }

    if(soundWav.getSizeHeader() % size_sound_in_value == 0)
    {
        printf("The file size is not divisible by %i\n", size_sound_in_value);
        return;
    }

    if(soundWav.getSizeHeader() % size_sound_in_value != 0)
    {
        printf("The file size is not divisible by %i\n", size_sound_in_value);
        return;
    }

    sizeSample = size_sound_in_value;
    sampleNb = (int)(soundWav.getSizeHeader() / sizeSample);
    angle = 180 / (sampleNb - 1);

    bufferSound = new short [sizeData];
    soundWav.readData(bufferSound, sizeData);

    emptyBuffer = new short [sizeSample];
    memset(bufferSound, 0, sizeSample*sizeof(short));
    isInit = true;
}

short* SoundReaderHrtf::getSpatializedSound(int idx)
{
    short * pointer_return;
    if(!isInit && idx < 0 || idx > 180)
    {
        return emptyBuffer;
    }
    else
    {
        int offset = round((float) idx / (float) angle) * sizeSample;
        pointer_return = bufferSound + offset;
    }
    return pointer_return;
}


