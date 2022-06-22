//
// Created by ubuntu on 21/06/22.
//

#include "sound_reader.h"



SoundReader::SoundReader(const char* file, const int sizechunks) : sizeAudioChunkSample(sizechunks)
{
    SoundWav soundWav((char*)file);
    soundWav.readHeader();
    sizeData = soundWav.getSizeData();
    if(soundWav.getNbChannel() != 1)
    {
        printf("Error, the file %s is not a monophonic sound");
        return;
    }
    int numberChunks = ceil((sizeData) / sizechunks);
    bufferSound = new short [numberChunks * 2 * sizechunks];
    auto* tempArray = new float [numberChunks * sizechunks];
    memset(bufferSound, 0, numberChunks*sizechunks*sizeof(float));
    lastChunkPtr =  bufferSound + sizechunks * (numberChunks - 1);
    soundWav.readData(tempArray, sizeData);
    for(int i = 0; i < numberChunks * sizechunks; i++)
    {
        bufferSound[2*i] = (short)tempArray[i];
        bufferSound[2*i+1] = (short)tempArray[i];
    }
    this->currentChunkPtr = bufferSound;
}

bool SoundReader::isReading()
{
    if(currentChunkPtr != nullptr)
        return true;
    else
        return false;
}

void SoundReader::start()
{
    currentChunkPtr = bufferSound;
}

void *SoundReader::pull_buffer()
{
    void* pointer_result = NULL;
    pointer_result = currentChunkPtr;
    if(currentChunkPtr == lastChunkPtr)
        currentChunkPtr = nullptr;
    else
        currentChunkPtr += sizeAudioChunkSample;
    return pointer_result;
}

SoundReader::~SoundReader()
{
    delete[] bufferSound;
}