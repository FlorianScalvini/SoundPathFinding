//
// Created by ubuntu on 20/06/22.
//

#ifndef STAG_LAV_MANAGER_H
#define STAG_LAV_MANAGER_H

#include "../sound/sound_reader.h"
#include "../path/pathFinding.h"
#include "../path/graph.h"
#include "lav_vocal.h"
#include "lav_sonifier.h"
#include "lav_constants.h"

#include "lav_video_processor.h"

#include <vector>

enum ENUM_STATE
{
    SCAN_ENV = 0,
    WAIT_DST = 1,
    NEAR_TARGET = 2,
    WAIT_USER = 3,
    IN_TRANSIT = 4,
    CHECK_NEIGHBOR = 5,
};

class lavManager {
public:
    static void init();
    static void setDst(unsigned int dst);
    static void setFirstNode(unsigned int src);
    static bool isVoiceControl();


    static void waitDst();
    static void inTransit();
    static void nearSpecialTarget();
    static void scanEnv();
    static void nearTarget();
    static void process();
    static void* start_path_manager(void* args);
    static void start_thread_path_manager();
    static void release();

private:
    static bool close_thread;
    static int state;
    static unsigned int currentNode;
    static unsigned int dst;
    static PathFinding* path;
    static Graph* graph;
    static std::vector<SoundReader> sounds;
    static SoundReader* sound;


};


#endif //STAG_LAV_MANAGER_H
