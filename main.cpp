#include <iostream>
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#include "sonifier/lav_video_processor.h"
#include "sonifier/lav_log.h"
#include "Stag.h"
#include "path/Graph.h"

void drawMarkers(cv::Mat* image, const vector<Marker> &markers)
{

    for (int i = 0; i < markers.size(); i++)
    {
        vector<cv::Point2d> corners = markers[i].corners;
        cv::Point2d center = markers[i].center;

        cv::circle(*image, cv::Point(corners[0].x, corners[0].y), 6, cv::Scalar(255, 255, 255), -1, cv::LINE_AA);
        for (int j = 0; j < 4; j++)
            cv::line(*image, cv::Point(corners[j].x, corners[j].y), cv::Point(corners[(j + 1) % 4].x, corners[(j + 1) % 4].y), cv::Scalar(255, 255, 255), 3, cv::LINE_AA);

        cv::circle(*image, cv::Point(corners[0].x, corners[0].y), 5, cv::Scalar(50, 255, 50), -1, cv::LINE_AA);
        for (int j = 0; j < 4; j++)
            cv::line(*image, cv::Point(corners[j].x, corners[j].y), cv::Point(corners[(j + 1) % 4].x, corners[(j + 1) % 4].y), cv::Scalar(50, 255, 50), 2, cv::LINE_AA);

        cv::circle(*image, cv::Point(center.x, center.y), 6, cv::Scalar(255, 255, 255), -1, cv::LINE_AA);
        cv::circle(*image, cv::Point(center.x, center.y), 5, cv::Scalar(50, 255, 50), -1, cv::LINE_AA);

        cv::putText(*image, std::to_string(markers[i].id), center, cv::FONT_HERSHEY_DUPLEX, 2, cv::Scalar(255, 255, 255), 5, cv::LINE_AA);
        cv::putText(*image, std::to_string(markers[i].id), center, cv::FONT_HERSHEY_DUPLEX, 2, cv::Scalar(50, 50, 255), 2, cv::LINE_AA);
    }
}

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
        drawMarkers(&img,stag.markers);
        cv::imshow("IMAGE", img);

        cv::waitKey(1);
    }
    lavVideoProcessor::release();
    return 0;
}
