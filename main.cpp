#include <iostream>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#include "sonifier/lav_video_processor.h"
#include "sonifier/lav_log.h"
#include "Stag.h"
#include "path/graph.h"


int main() {


    lavVideoProcessor::init();
    lavLog::LAVLOG("lavVideoProcessor::start_thread_video_stream\n");
    lavVideoProcessor::start_thread_video_stream();
    Stag stag(15, 7, true);
    cv::Mat img;
    while(true)
    {
        while (!lavVideoProcessor::_colorProcessingDone) {
            usleep(100);
        }
        std::cout<<lavVideoProcessor::_inputMatColor.cols<<std::endl;
        cv::Mat img;
        cv::cvtColor(lavVideoProcessor::_inputMatColor, img, cv::COLOR_BGR2GRAY);
        cv::resize(img, img, cv::Size(1280,720));
        stag.detectMarkers(img);
        cv::imshow("IMAGE", img);

        cv::waitKey(1);
    }
    lavVideoProcessor::release();
    return 0;
}
