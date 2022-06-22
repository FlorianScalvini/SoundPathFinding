//
// Created by ubuntu on 20/06/22.
//

#include "lav_manager.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


int lavManager::state = NULL;
unsigned int lavManager::dst = NULL;
PathFinding* lavManager::path = nullptr;
Graph* lavManager::graph = nullptr;
std::vector<SoundReader> lavManager::sounds;
SoundReader* lavManager::sound = nullptr;
unsigned int lavManager::currentNode;
bool lavManager::close_thread;

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

void lavManager::nearSpecialTarget()
{
    lavVocal::push_buffer(-1);
    usleep(100000);
}

void lavManager::nearTarget()
{
    path->update();
    if(currentNode == dst)
    {
        lavVocal::push_buffer(3);
        state = WAIT_DST;
    }
    else{
        lavVocal::push_buffer(1);
        state = SCAN_ENV;
    }

}

void lavManager::scanEnv() {
    std::vector<MarkerDetection> markers;
    MarkerDetection* mrk = nullptr;
    for(auto marker: markers)
    {
        if(path->changeToClosestNode(marker.label_i))
        {
            mrk = &marker;
        }
    }
    if(mrk!= nullptr)
    {
        state = IN_TRANSIT;
    }
}

void lavManager::inTransit()
{
    cv::Mat output = cv::Mat(cv::Size(COLOR_FRAME_WIDTH, COLOR_FRAME_HEIGHT), CV_8UC1);
    output.setTo(cv::Scalar(0));
    std::vector<MarkerDetection> markers;
    MarkerDetection* mrk = nullptr;
    for(auto marker: markers)
    {
        if(path->changeToClosestNode(marker.label_i))
        {
            mrk = &marker;
        }
    }
    if(mrk != nullptr)
    {
        for(int x = mrk->x_pixel - 7; x < mrk->x_pixel + 7; x++)
        {
            for(int y = mrk->y_pixel - 7; y < mrk->y_pixel + 7; y++)
            {
                if(x >= 0 && y >= 0 && x < COLOR_FRAME_WIDTH && y < COLOR_FRAME_WIDTH)
                    output.at<unsigned char>(y, x);
            }
        }
    }
    cv::resize(output, output, cv::Size(FRAME_WIDTH_SONIFIED, FRAME_HEIGHT_SONIFIED), 0, 0, 0);
    lavSonifier::sonify(nullptr);
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
        case NEAR_TARGET:
            nearTarget();
            break;
        case SCAN_ENV:
            scanEnv();
            break;
        default:
            return;
    }
}

void lavManager::waitDst()
{
    lavVocal::push_buffer(0);
    std::string char_dst;
    std::getline(std::cin, char_dst);
    // If the string is a number
    if(std::all_of(char_dst.begin(), char_dst.end(), [](const char i){return std::isdigit(i);}))
    {
        lavManager::setDst(std::stoi(char_dst));
        if(path->newPath(currentNode, dst));
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
    while (!close_thread) {
        lavManager::process();
    }
    return nullptr;
}

void lavManager::release()
{
    close_thread = true;
}

void lavManager::start_thread_path_manager() {
    close_thread = false;
    pthread_t thread_video_processing;
    pthread_create(&thread_video_processing, nullptr, start_path_manager, (void*)nullptr);
}

