//
// Created by ubuntu on 21/06/22.
//

#include "sound_reader.h"



SoundReader::SoundReader(const char* file, const int sizechunks) : sizeAudioChunkSample(sizechunks)
{
    SoundWav soundWav((char*)file);
    soundWav.readHeader();
    sizeData = soundWav.getSizeData();
    int numberChunks = ceil((sizeData) / sizechunks);
    bufferSound = new short [numberChunks*sizechunks];
    memset(bufferSound, 0, numberChunks*sizechunks*sizeof(float));
    lastChunkPtr =  bufferSound + sizechunks * (numberChunks - 1);
    soundWav.readData(bufferSound, sizeData);
    this->currentChunkPtr = bufferSound;
}

bool SoundReader::isReading()
{
    if(currentChunkPtr == bufferSound)
        return true;
    else
        return false;
}

void *SoundReader::pull_buffer(bool rst)
{
    void* pointer_result = NULL;
    if(rst)
    {
        currentChunkPtr = bufferSound;
    }
    pointer_result = currentChunkPtr;
    if(currentChunkPtr == lastChunkPtr)
        currentChunkPtr = bufferSound;
    else
        currentChunkPtr += sizeAudioChunkSample;
    return pointer_result;
}

