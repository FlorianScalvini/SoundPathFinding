//
// Created by florian on 02/07/2021.
//
#ifndef LAV_REALSENSE_CAMERA_H
#define LAV_REALSENSE_CAMERA_H

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <unistd.h>
#include "lav_video_capture.h"
#include "lav_constants.h"

#define DEC_FILTER 1
#define SPAT_FILTER 1
#define TEMP_FILTER 1

#define HEIGHT 240
#define WIDTH 320


class lavRealsenseCamera: public lavVideoCapture {

private:
    cv::Mat depthCapture;
    cv::Mat depthTransfert;
    cv::Mat depthOutput;

    cv::Mat colorCapture;
    cv::Mat colorTransfert;
    cv::Mat colorOutput;

    cv::Mat _tmpMat;
    cv::VideoCapture* _capture;

    rs2::pipeline pipeRealSense;
    rs2::config cfg;

    rs2::decimation_filter dec_filter;  // Decimation - reduces depth frame density
    rs2::spatial_filter spat_filter;    // Spatial    - edge-preserving spatial smoothing
    rs2::temporal_filter temp_filter;   // Temporal   - reduces temporal noise

    const std::string disparity_filter_name = "Disparity";
    rs2::disparity_transform depth_to_disparity = rs2::disparity_transform(true);
    rs2::disparity_transform disparity_to_depth = rs2::disparity_transform(false);
    std::vector<rs2::filter> filters;

    pthread_mutex_t _mutexNewColorFrame;
    pthread_mutex_t _mutexNewDepthFrame;

    bool g_bDeviceFound;
    bool run_acquisition;
    bool _aNewColorFrameHasBeenAcquired;
    bool _aNewDepthFrameHasBeenAcquired;
    int cptAcquisition;

    void configureDepthCamera();
    void check_for_camera();
    void start_realsense_acquisition();
    void realsense_acquisition();
    void transfertNewFrame();


    static void* start_realsense_acquisition_inThread(void* args);
    static void start_thread_realsense_acquisition(void* args);
    static cv::Mat frame_to_mat(const rs2::frame& f);

    lavRealsenseCamera();

public:

    static lavRealsenseCamera* __singleton;
    static lavRealsenseCamera* getSingleton();
    cv::Mat getNextFrame();
    cv::Mat getNextFrameColor();
    void release();



};

#endif //LAV_REALSENSE_CAMERA_H