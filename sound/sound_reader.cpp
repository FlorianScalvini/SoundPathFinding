//
// Created by ubuntu on 21/06/22.
//

#include "sound_reader.h"



SoundReader::SoundReader() {
    currentChunkPtr = nullptr;
    lastChunkPtr = nullptr;
    numberChunks = 0;
    sizeAudioChunkSample = 0;
    sizeData = 0;
    isInit = false;
    bufferSound = nullptr;
}


int SoundReader::init(char* file, int sizechunks) {
    sizeAudioChunkSample = sizechunks;
    SoundWav soundWav((char *) file);
    soundWav.readHeader();
    sizeData = soundWav.getSizeData();
    if(soundWav.getNbChannel() != 1)
    {
        printf("Error, the file %s is not a monophonic sound");
        return EXIT_FAILURE;
    }
    numberChunks = ceil((sizeData) / sizeAudioChunkSample);
    bufferSound = new short [numberChunks * 2 * sizeAudioChunkSample];
    memset(bufferSound, 0, numberChunks*2*sizeAudioChunkSample*sizeof(short));
    auto* tempArray = new short [numberChunks * sizeAudioChunkSample];
    soundWav.readData(tempArray, sizeData);
    return EXIT_SUCCESS;
}

bool SoundReader::isReading()
{
    if(currentChunkPtr == nullptr || !isInit)
        return false;
    else
        return true;
}

void SoundReader::start()
{
    currentChunkPtr = bufferSound;
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
