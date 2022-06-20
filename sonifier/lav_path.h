//
// Created by ubuntu on 20/06/22.
//

#ifndef STAG_LAV_PATH_H
#define STAG_LAV_PATH_H

#include "../sound/wav_helpers.h"
#include "../path/pathFinding.h"
#include "../path/graph.h"

struct soundData
{
    Wav_header header;
    char* data;
};

enum ENUM_STATE
{
    WAIT_DST = 0,
    CHECK_NEIGHBOR = 0,
    NEAR_MARKER = 0,
    CHECK_NEIGHBOR = 0,
    END = 0,
};

class Lav_path {
    static void init();
    static void setDst(unsigned int dst);
    static void setFirstNode(unsigned int src);

private:
    static char * dataCheck;
    static int state;
    static unsigned int dst;
    static PathFinding* path;
    static Graph* graph;
    static char* soundControl;
};


#endif //STAG_LAV_PATH_H
