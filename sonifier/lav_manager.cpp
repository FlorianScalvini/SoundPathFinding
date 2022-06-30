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
    state = INIT_STATE;
    graph = new Graph("/home/ubuntu/CLionProjects/SoundPathFinding/path/graph.txt");
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
    if(path == nullptr)
    {
        std::cerr<<"Pointer to path is null" <<std::endl;
        return;
    }
    else
    {
        currentNode = path->getCurrentNode()->label;
        lavVocal::start(4);
        usleep(100000);
        state = NEAR_TARGET;
        std::cout<<"State : Near target special -> Near target"<<std::endl;
    }
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
        lavVocal::start(2);
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
        std::cout<<"State : Near target -> In transit"<<std::endl;
        path->showPath();
        lavVideoProcessor::start();
        state = IN_TRANSIT;
    }
}

void lavManager::inTransit()
{
    int angle = -1;
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

        if(mrk->distance > 100 && mrk ->distance < 800)
        {
            int classLink = graph->getClasse(mrk->label_i, path->getNextNode());
            if(mrk->distance < 600 && classLink > 0)
            {
                std::cout<<"State : In transit -> Near special target"<<std::endl;
                state = NEAR_TARGET_SPECIAL;
            }
            else if(classLink == 0)
            {
                std::cout<<"State : In transit -> Near target"<<std::endl;
                state = NEAR_TARGET;
            }
            dataOut.sonify.setTo(cv::Scalar(0));
        }
        else
        {
            angle = mrk->angle;
        }

    }



    //cv::resize(outSonify, outSonify, cv::Size(FRAME_WIDTH_SONIFIED, FRAME_HEIGHT_SONIFIED), 0, 0, 0);
    //cv::imshow("sonify", outSonify);
    //cv::waitKey(1);
    //cv::imshow("fff", dataOut.sonify);
    //cv::waitKey(1);
    lavSonifier::sonify(&dataOut.sonify, angle);
}

void lavManager::searchFirstNode()
{
    DataVideoProcessing dataOut = lavVideoProcessor::pull_data();
    unsigned short depth = 0xFFFF;
    int idxNode = -1;
    for(int i =0; i < dataOut.data_path.size(); i++)
    {
        if(depth > dataOut.data_path[i].distance)
        {
            depth = dataOut.data_path[i].distance;
            idxNode = dataOut.data_path[i].label_i;
        }
    }

    if(idxNode != -1 && path->setCurrentNode(idxNode))
    {
        currentNode = idxNode;
        std::cout<<"First marker found !" << std::endl;
        state = WAIT_DST;
    }
}

void lavManager::process()
{
    while(lavVocal::isReading())
    {
        usleep(250);
    }
    switch (state) {
        case WAIT_DST:
            waitDst();
            break;
        case INIT_STATE:
            searchFirstNode();
            break;
        case IN_TRANSIT:
            inTransit();
            break;
        case NEAR_TARGET_SPECIAL:
            nearSpecialTarget();
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
    lavVideoProcessor::stop();
    lavVocal::start(1);
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
            lavVideoProcessor::start();
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
    lavVideoProcessor::start();
    std::cout<<"Checking first node ..... : "<<std::endl;
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

