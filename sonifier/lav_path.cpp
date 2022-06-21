//
// Created by ubuntu on 20/06/22.
//

#include "lav_path.h"

int lavPath::state = NULL;
unsigned int lavPath::dst = NULL;
PathFinding* lavPath::path = nullptr;
Graph* lavPath::graph = nullptr;
std::vector<SoundReader> lavPath::sounds;
SoundReader* lavPath::sound = nullptr;



void lavPath::init()
{
    state = WAIT_DST;
    graph = new Graph("/home/florian/CLionProjects/SoundPathFinding/path/graph.txt");
    path = new PathFinding(graph);
}

bool lavPath::isVoiceControl()
{
    if(state != IN_TRANSIT && state != WAIT_USER)
        return true;
    else
        return false;
}

void lavPath::inTransit()
{
    lavSonifier::
}



void lavPath::process()
{
    if(LavVocal::isReading())
    {
        usleep(500);
    }
    if(state = IN_TRANSIT)
    {

    }
    if(state != IN_TRANSIT && state != WAIT_USER)
    {
        state
    }

}

void lavPath::setDst(unsigned int dst) {
    lavPath::dst = dst;
    state = CHECK_NEIGHBOR;
}

void lavPath::setFirstNode(unsigned int src)
{
    path->newPath(src, dst);
    state =
}
