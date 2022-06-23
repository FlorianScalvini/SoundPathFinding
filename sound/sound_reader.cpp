//
// Created by ubuntu on 21/06/22.
//

#include "sound_reader.h"



SoundReader::SoundReader(const char* file, const int sizechunks)
{
    sizeAudioChunkSample = sizechunks;
    SoundWav soundWav((char*)file);
    soundWav.readHeader();
    sizeData = soundWav.getSizeData();
    if(soundWav.getNbChannel() != 1)
    {
        printf("Error, the file %s is not a monophonic sound");
        return;
    }
    int numberChunks = ceil((sizeData) / sizechunks);
    short* buffer = new short [numberChunks * 2 * sizechunks];


    auto* tempArray = new short [numberChunks * sizechunks];
    memset(buffer, 0, numberChunks*sizechunks*sizeof(float));

    soundWav.readData(tempArray, sizeData);
    for(int i = 0; i < numberChunks * sizechunks; i++)
    {
        buffer[2*i] = (short)tempArray[i];
        buffer[2*i+1] = (short)tempArray[i];
    }

    this->currentChunkPtr = *bufferSound;
    lastChunkPtr =  buffer + sizechunks * (numberChunks - 1);
    bufferSound = &buffer;
    std::cout<<sizechunks<<std::endl;
}

bool SoundReader::isReading()
{

    if(currentChunkPtr == nullptr)
        return false;
    else
        return true;
}

void SoundReader::start()
{
    currentChunkPtr = *bufferSound;
}

void* SoundReader::pull_buffer()
{
    void* pointer_result = currentChunkPtr;
    if(currentChunkPtr == lastChunkPtr)
        currentChunkPtr = nullptr;
    else
        currentChunkPtr += sizeAudioChunkSample;
    return pointer_result;
}

SoundReader::~SoundReader()
{
    delete[] *bufferSound;
}