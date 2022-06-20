//
// Created by ubuntu on 20/06/22.
//

#include "lav_path.h"

int Lav_path::state = NULL;
unsigned int Lav_path::dst = NULL;
PathFinding* Lav_path::path = nullptr;
Graph* Lav_path::graph = nullptr;
char* Lav_path::soundCheck = nullptr;

void Lav_path::init()
{
    state = WAIT_DST;
    graph = new Graph("/home/florian/CLionProjects/SoundPathFinding/path/graph.txt");
    path = new PathFinding(graph);
}

void Lav_path::emitSoundControl()
{
    WavHelpers* wavHelpers;
    if(state == CHECK_NEIGHBOR)
    {
        wavHelpers = new WavHelpers("");
        soundControl = new ;
    }
}

void Lav_path::setDst(unsigned int dst) {
    Lav_path::dst = dst;

    state = CHECK_NEIGHBOR;
}

void Lav_path::setFirstNode(unsigned int src)
{
    path->newPath(src, dst);
    state =
}

void Lav_path::
