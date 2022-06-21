//
// Created by ubuntu on 20/06/22.
//

#ifndef STAG_LAV_PATH_H
#define STAG_LAV_PATH_H

#include "../sound/sound_reader.h"
#include "../path/pathFinding.h"
#include "../path/graph.h"
#include "lav_vocal.h"
#include "lav_sonifier.h"
#include "lav_constants.h"
#include <vector>


enum ENUM_STATE
{
    WAIT_DST = 0,
    IN_TRANSIT = 1,
    WAIT_USER = 2,
    CHECK_NEIGHBOR = 3,
};

class lavPath {
public:
    static void init();
    static void setDst(unsigned int dst);
    static void setFirstNode(unsigned int src);
    static bool isVoiceControl();
    static void* pull_buffer();

    static void* start_path_process(void* args);
    static void start_thread_path_process() ;

private:
    static int state;
    static unsigned int dst;
    static PathFinding* path;
    static Graph* graph;
    static std::vector<SoundReader> sounds;
    static SoundReader* sound;


};


#endif //STAG_LAV_PATH_H
