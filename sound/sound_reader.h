//
// Created by ubuntu on 21/06/22.
//

#ifndef STAG_SOUND_READER_H
#define STAG_SOUND_READER_H

#include "sound_wav.h"


class SoundReader {
public:
    explicit SoundReader(const char* file, const int sizechunks);
    ~SoundReader();
    void* pull_buffer();
    void start();
    bool isReading();
private:
    const int sizeAudioChunkSample;
    int sizeData;
    short * bufferSound;
    short *lastChunkPtr;
    short *currentChunkPtr;
};


#endif //STAG_SOUND_READER_H
