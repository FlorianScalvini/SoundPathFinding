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
    graph = new Graph("/home/florian/CLionProjects/SoundPathFinding/path/graph.txt");
    graph->showGraph();
    path = new PathFinding(graph);
    currentNode = 1;
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
    currentNode = path->getCurrentNode()->label;
    path->update();
    path->showPath();
    std::cout<<currentNode<< " "<<dst<<std::endl;
    if(currentNode == dst)
    {
        std::cout<<"Destination atteinte"<<std::endl;
        lavVocal::push_buffer(3);
        std::cout<<"State : Near target -> Wait Dst"<<std::endl;
        state = WAIT_DST;
    }
    else{
        lavVocal::push_buffer(1);
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
    std::cout<<"inTransir: "<<std::endl;
    lavVideoProcessor::startSound();
    cv::Mat output = cv::Mat(cv::Size(COLOR_FRAME_WIDTH, COLOR_FRAME_HEIGHT), CV_8UC1);
    output.setTo(cv::Scalar(0));
    DataVideoProcessing dataOut = lavVideoProcessor::pull_data();
    PathOut* mrk = nullptr;
    for(auto pathData: dataOut.data_path)
    {
        std::cout<<"Marker : " << pathData.distance << " "<<pathData.x_pixel<< " " << pathData.y_pixel <<" " <<pathData.label_i;
        /*
        if(path->changeToClosestNode(pathData.label_i))
        {
            mrk = &pathData;
        }
         */
        if(pathData.label_i == 0)
        {
            mrk = &pathData;
            break;
        }
    }
    if(mrk != nullptr)
    {
        std::cout<<"Marker select: " << mrk->label_i <<std::endl;
        for(int x = mrk->x_pixel - 10; x < mrk->x_pixel + 10; x++)
        {
            for(int y = mrk->y_pixel - 10; y < mrk->y_pixel + 10; y++)
            {
                if(x >= 0 && y >= 0 && x < COLOR_FRAME_WIDTH && y < COLOR_FRAME_WIDTH)
                    output.at<unsigned char>(y, x) = 255;
            }
        }
        if(mrk->distance < 800 && mrk->distance > 100)
        {
            std::cout<<"State : In transit -> Near target"<<std::endl;
            state = NEAR_TARGET;
        }
    }
    cv::resize(output, output, cv::Size(FRAME_WIDTH_SONIFIED, FRAME_HEIGHT_SONIFIED), 0, 0, 0);
    cv::imshow("sonify", output);
    //lavSonifier::sonify(&output);
}


void lavManager::process()
{
    while(lavVocal::isReading())
    {
        usleep(500);
    }
    std::cout<<"New"<<std::endl;
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
    lavVideoProcessor::stopSound();
    lavVocal::push_buffer(0);
    std::string char_dst;
    std::cout<<"Destination: "<<std::endl;
    std::getline(std::cin, char_dst);
    // If the string is a number
    if(!char_dst.empty() && std::all_of(char_dst.begin(), char_dst.end(), [](const char i){return std::isdigit(i);}))
    {
        lavManager::setDst(std::stoi(char_dst));
        if(path->newPath(currentNode, dst));
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

