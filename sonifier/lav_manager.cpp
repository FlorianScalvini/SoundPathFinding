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
unsigned int lavManager::currentNode;
bool lavManager::close_thread;

void lavManager::init()
{
    state = WAIT_DST;
    graph = new Graph("/home/ubuntu/CLionProjects/pathFinder/path/graph.txt");
    graph->showGraph();
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
    lavVocal::start(-1);
    usleep(100000);
}

void lavManager::nearTarget()
{
    currentNode = path->getCurrentNode()->label;
    path->update();
    path->showPath();
    std::cout<<currentNode<< " "<<dst<<std::endl;
    if(currentNode == dst)
    {
        std::cout<<"Destination atteinte"<<std::endl;
        lavVocal::start(3);
        std::cout<<"State : Near target -> Wait Dst"<<std::endl;
        state = WAIT_DST;
    }
    else{
        lavVocal::start(1);
        state = SCAN_ENV;
        std::cout<<"State : Near target -> Scan env"<<std::endl;
    }

}

void lavManager::scanEnv() {
    DataVideoProcessing dataOut = lavVideoProcessor::pull_data();
    PathOut* mrk = nullptr;
    for(auto marker: dataOut.data_path)
    {
        if(path->changeToClosestNode(marker.label_i))
        {
            mrk = &marker;
        }
    }
    if(mrk!= nullptr)
    {
        std::cout<<"State : In transit -> Near target"<<std::endl;
        path->showPath();
        lavVideoProcessor::startSound();
        state = IN_TRANSIT;
    }
}

void lavManager::inTransit()
{



    DataVideoProcessing dataOut = lavVideoProcessor::pull_data();
    PathOut* mrk = nullptr;
    for(int i =0; i < dataOut.data_path.size(); i++)
    {
        if(dataOut.data_path[i].label_i == path->getCurrentNode()->label)
        {
            mrk = &dataOut.data_path[i];
            break;
        }
    }

    cv::Mat outSonify = cv::Mat(cv::Size(COLOR_FRAME_WIDTH, COLOR_FRAME_HEIGHT), CV_8UC1);
    outSonify.setTo(cv::Scalar(0));
    if(mrk != nullptr)
    {

        std::cout<<"Marker select: " << mrk->label_i <<" " <<mrk->x_pixel<< " " << mrk->y_pixel <<" "<<mrk->distance <<std::endl;
        if(mrk->distance < 800 && mrk->distance > 100)
        {
            std::cout<<"State : In transit -> Near target"<<std::endl;
            state = NEAR_TARGET;
        }
        else
        {
            for(int x = mrk->x_pixel - 10; x < mrk->x_pixel + 10; x++)
            {
                for(int y = mrk->y_pixel - 10; y < mrk->y_pixel + 10; y++)
                {
                    if(x >= 0 && y >= 0 && x < COLOR_FRAME_WIDTH && y < COLOR_FRAME_WIDTH)
                        outSonify.at<unsigned char>(y, x) = 255;
                }
            }
        }

    }
    cv::resize(outSonify, outSonify, cv::Size(FRAME_WIDTH_SONIFIED, FRAME_HEIGHT_SONIFIED), 0, 0, 0);
    cv::imshow("sonify", outSonify);
    cv::waitKey(1);
    lavSonifier::sonify(&outSonify);
}

void lavManager::searchFirstNode()
{
    DataVideoProcessing dataOut = lavVideoProcessor::pull_data();
    short depth = 0xFFFF;
    int idxNode = 0xFFFFFFFF;
    for(int i =0; i < dataOut.data_path.size(); i++)
    {
        if(depth > dataOut.data_path[i].distance)
        {
            depth = dataOut.data_path[i].distance;
            idxNode = dataOut.data_path[i].label_i;
        }
    }
}

void lavManager::process()
{
    while(lavVocal::isReading())
    {
        usleep(500);
    }
    switch (state) {
        case INIT_STATE:
            searchFirstNode();
            break;
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
    lavVideoProcessor::stopSound();
    lavVocal::start(0);
    std::string char_dst;
    std::cout<<"Destination: "<<std::endl;
    std::getline(std::cin, char_dst);
    // If the string is a number
    if(!char_dst.empty() && std::all_of(char_dst.begin(), char_dst.end(), [](const char i){return std::isdigit(i);}))
    {
        lavManager::setDst(std::stoi(char_dst));
        if(path->newPath(dst));
        {
            path->showPath();
            state = IN_TRANSIT;
            lavVideoProcessor::startSound();
            path->update();
            std::cout<<"Start sonification vers cible "<< path->getCurrentNode()->label<< std::endl;
        }
    }
    else
        usleep(100000);
}

void lavManager::setDst(unsigned int dst) {
    lavManager::dst = dst;
    state = CHECK_NEIGHBOR;
}


void* lavManager::start_path_manager(void* args) {
    lavVocal::start(0);
    lavVideoProcessor::startSound();
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
    pthread_join(thread_video_processing, nullptr);
}

