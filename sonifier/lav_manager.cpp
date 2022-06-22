//
// Created by ubuntu on 20/06/22.
//

#include "lav_manager.h"

int lavManager::state = NULL;
unsigned int lavManager::dst = NULL;
PathFinding* lavManager::path = nullptr;
Graph* lavManager::graph = nullptr;
std::vector<SoundReader> lavManager::sounds;
SoundReader* lavManager::sound = nullptr;
unsigned int lavManager::currentNode;

void lavManager::init()
{
    state = WAIT_DST;
    graph = new Graph("/home/florian/CLionProjects/SoundPathFinding/path/graph.txt");
    path = new PathFinding(graph);
    currentNode = 0;
}

bool lavManager::isVoiceControl()
{
    if(state != IN_TRANSIT)
        return true;
    else
        return false;
}

void lavManager::nearSpecialTarget(int indice)
{

}

void lavManager::inTransit()
{
    lavSonifier::sonify(nullptr);
}

bool isNumber(const std::string& s)
{
    for (char const &ch : s) {
        if (std::isdigit(ch) == 0)
            return false;
    }
    return true;
}

void lavManager::process()
{
    while(sound->isReading())
    {
        usleep(500);
    }
    switch (state) {
        case WAIT_DST:
            waitDst();
            break;
        case IN_TRANSIT:
            inTransit();
            break;
        case NEAR_SPECIAL_TARGET:
            nearSpecialTarget();
            break;
        case SCAN_ENV:

            break;
        default:
            return;
    };
}

void lavManager::waitDst()
{
    lavVocal::push_buffer(0);
    std::string char_dst;
    std::getline(std::cin, char_dst);
    if(isNumber(char_dst))
    {
        lavManager::setDst(std::stoi(char_dst));
        path->newPath(currentNode, dst);
        state = IN_TRANSIT;
    }
    else
        usleep(100000);
}

void lavManager::setDst(unsigned int dst) {
    lavManager::dst = dst;
    state = CHECK_NEIGHBOR;
}


void* lavManager::start_path_manager(void* args) {
    while (1) {
        lavManager::process();
    }
    return nullptr;
}


void lavManager::start_thread_path_manager() {
    //usleep(10000);
    pthread_t thread_video_processing;
    pthread_create(&thread_video_processing, nullptr, start_path_manager, (void*)nullptr);
}

