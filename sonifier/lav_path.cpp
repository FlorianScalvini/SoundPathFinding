//
// Created by ubuntu on 20/06/22.
//

#include "lav_path.h"

int Lav_path::state = NULL;
unsigned int Lav_path::dst = NULL;
PathFinding* Lav_path::path = nullptr;
Graph* Lav_path::graph = nullptr;
std::vector<SoundReader> Lav_path::sounds;


void Lav_path::init()
{
    state = WAIT_DST;
    graph = new Graph("/home/florian/CLionProjects/SoundPathFinding/path/graph.txt");
    path = new PathFinding(graph);
    sounds.emplace_back(SoundReader("/home/ubuntu/Bureau/HrtfConvo/person.wav", SIZE_AUDIO_CHUNK_IN_VALUE));
}

void* Lav_path::emitSoundControl()
{
    if(state == CHECK_NEIGHBOR)
    {
        sounds[0].isReading()
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

