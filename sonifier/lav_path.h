//
// Created by ubuntu on 20/06/22.
//

#ifndef STAG_LAV_PATH_H
#define STAG_LAV_PATH_H

#include "../sound/wav_helpers.h"
struct soundData
{
    Wav_header header;
    char* data;
};

enum ENUM_STATE
{
    WAIT_PATH = 0,
    NEAR_MARKER = 0,
    CHECK_NEIGHBOR = 0,
    END = 0,
};

class Lav_path {
    static void init();
private:
    static char * dataCheck;
    static int state;

};


#endif //STAG_LAV_PATH_H
