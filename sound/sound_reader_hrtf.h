//
// Created by ubuntu on 27/06/22.
//

#ifndef STAG_SOUND_READER_HRTF_H
#define STAG_SOUND_READER_HRTF_H

#include "sound_reader.h"

class SoundReaderHrtf : public SoundReader{
public:
    SoundReaderHrtf();
    void start(unsigned int idx) override;
    int init(char* file, int sizechunks) override;

private:
    using SoundReader::start;
    int sizeSample; // Taille d'un echantillion de la base de donnée
};


#endif //STAG_SOUND_READER_HRTF_H
