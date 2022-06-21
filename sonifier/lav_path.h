//
// Created by ubuntu on 20/06/22.
//

#ifndef STAG_LAV_PATH_H
#define STAG_LAV_PATH_H

#include "../sound/sound_reader.h"
#include "../path/pathFinding.h"
#include "../path/graph.h"
#include "lav_constants.h"
#include <vector>


enum ENUM_STATE
{
    WAIT_DST = 0,
    CHECK_NEIGHBOR = 0,
};

class Lav_path {
    static void init();
    static void setDst(unsigned int dst);
    static void* emitSoundControl();
    static void setFirstNode(unsigned int src);

private:
    static int state;
    static unsigned int dst;
    static PathFinding* path;
    static Graph* graph;
    static std::vector<SoundReader> sounds;

};


#endif //STAG_LAV_PATH_H
