//
// Created by florian on 02/07/2021.
//

#include "lav_realsense_camera.h"


lavRealsenseCamera* lavRealsenseCamera::__singleton = nullptr;

lavRealsenseCamera* lavRealsenseCamera::getSingleton()
{
    if (__singleton ==nullptr) {
        __singleton = new lavRealsenseCamera();
    }
    return __singleton;
}

lavRealsenseCamera::lavRealsenseCamera()
{
    lavLog::LAVLOG("Initialize LavRealsenseCamera ...");
    depthCapture = cv::Mat(HEIGHT, WIDTH, CV_16UC1);
    depthTransfert = cv::Mat(HEIGHT, WIDTH, CV_16UC1);
    depthOutput = cv::Mat(HEIGHT, WIDTH, CV_16UC1);

    _aNewDepthFrameHasBeenAcquired = false;
    _aNewColorFrameHasBeenAcquired = false;
    _mutexNewDepthFrame = PTHREAD_MUTEX_INITIALIZER;
    _mutexNewColorFrame = PTHREAD_MUTEX_INITIALIZER;
    start_thread_realsense_acquisition(this);

    cptAcquisition = 0;
}
cv::Mat lavRealsenseCamera::getNextFrame()
{
    while (!_aNewDepthFrameHasBeenAcquired) {
        usleep(250);
    }
    pthread_mutex_lock(&_mutexNewDepthFrame);
    depthTransfert.copyTo(depthOutput);
    _aNewDepthFrameHasBeenAcquired = false;
    pthread_mutex_unlock(&_mutexNewDepthFrame);

    return depthOutput;
}

cv::Mat lavRealsenseCamera::getNextFrameColor()
{
    while (!_aNewColorFrameHasBeenAcquired) {
        usleep(250);
    }
    pthread_mutex_lock(&_mutexNewColorFrame);
    colorTransfert.copyTo(colorOutput);
    _aNewColorFrameHasBeenAcquired = false;
    pthread_mutex_unlock(&_mutexNewColorFrame);

    return colorOutput;
}

void lavRealsenseCamera::release()
{
    run_acquisition = false;
    pipeRealSense.stop();
}

void lavRealsenseCamera::transfertNewFrame()
{
    //lavLog::LAVLOG("lavSoftKineticCamera::transfertNewFrame\n");

    pthread_mutex_lock(&_mutexNewDepthFrame);
    depthCapture.copyTo(depthTransfert);
    _aNewDepthFrameHasBeenAcquired = true;
    pthread_mutex_unlock(&_mutexNewDepthFrame);

    pthread_mutex_lock(&_mutexNewColorFrame);
    colorCapture.copyTo(colorTransfert);
    _aNewColorFrameHasBeenAcquired = true;
    pthread_mutex_unlock(&_mutexNewColorFrame);
}


void lavRealsenseCamera::configureDepthCamera()
{
    filters.push_back(dec_filter);
    filters.push_back(depth_to_disparity);
    filters.push_back(spat_filter);
    filters.push_back(temp_filter);
    filters.push_back(disparity_to_depth);


    //check_for_camera();
    //cfg.enable_device_from_file("");
    lavLog::LAVLOG("Check OK\n");
    cfg.enable_stream(RS2_STREAM_DEPTH, DEPTH_FRAME_WIDTH, DEPTH_FRAME_HEIGHT, RS2_FORMAT_Z16,30); // Set Stream Output + Resolution + Format + Framerate
    cfg.enable_stream(RS2_STREAM_COLOR, COLOR_FRAME_WIDTH, COLOR_FRAME_HEIGHT, RS2_FORMAT_RGB8,30);
}

void lavRealsenseCamera::check_for_camera()
{
    rs2::context context;
    auto test = context.query_all_sensors();
    rs2::device_list devices = context.query_devices();
    if (devices.size() == 0)
    {
        lavLog::LAVLOG("No device connected, please connect a RealSense device\n");
        usleep(500000);
        this->check_for_camera();
    }
    lavLog::LAVLOG("Device found\n");
    rs2::device device = devices.front(); // Select first device if multiple RealSense connected
    cfg.enable_device(device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
}


void lavRealsenseCamera::start_realsense_acquisition()
{
    configureDepthCamera();
    pipeRealSense.start(cfg);
    run_acquisition = true;
    lavLog::LAVLOG("Wait init camera ...");

    for(int i = 0; i < 30; i++)
    {
        pipeRealSense.wait_for_frames(); //Wait for all configured streams to produce a frame
    }
    lavLog::LAVLOG("Init OK");
    this->realsense_acquisition();

}

void lavRealsenseCamera::realsense_acquisition(){
    cv::Mat _inputMat;
    cv::Mat _inputColorMat;
    cv::Mat _smallInputMat;
    rs2::frameset frames;
    cv::Size size(WIDTH, HEIGHT);
    rs2::align align_to_color(RS2_STREAM_COLOR);

    while(run_acquisition)
    {
        frames = pipeRealSense.wait_for_frames();

        frames = align_to_color.process(frames);
        rs2::depth_frame depth = frames.get_depth_frame();
        rs2::frame color = frames.get_color_frame();

        for(rs2::filter filter : filters)
        {
            depth = depth.apply_filter(filter);
        }
        _inputMat = frame_to_mat(depth); // RS2_FORMAT_Z16 -> Mat C1
        cv::resize(_inputMat,_smallInputMat,size);
        _inputColorMat = frame_to_mat(color);
        _smallInputMat.convertTo(depthCapture,CV_16UC1,1);
        _inputColorMat.convertTo(colorCapture, CV_8UC3,1);
        transfertNewFrame();
    }
}


void* lavRealsenseCamera::start_realsense_acquisition_inThread(void* args)
{

    auto* thisPointer = (lavRealsenseCamera*) args;
    thisPointer->start_realsense_acquisition();
    return nullptr;
}




void lavRealsenseCamera::start_thread_realsense_acquisition(void* args)
{
    pthread_t thread_realsense_acquisition;
    pthread_create(&thread_realsense_acquisition, nullptr, start_realsense_acquisition_inThread, args);
}






cv::Mat lavRealsenseCamera::frame_to_mat(const rs2::frame& f)
{

    auto vf = f.as<rs2::video_frame>();
    const int w = vf.get_width();
    const int h = vf.get_height();

    if (f.get_profile().format() == RS2_FORMAT_BGR8)
    {
        return {cv::Size(w, h), CV_8UC3, (void*)f.get_data(), cv::Mat::AUTO_STEP};
    }
    else if (f.get_profile().format() == RS2_FORMAT_RGB8)
    {
        cv::Mat r_rgb = cv::Mat(cv::Size(w, h), CV_8UC3, (void*)f.get_data(), cv::Mat::AUTO_STEP);
        cv::Mat r_bgr;
        cv::cvtColor(r_rgb, r_bgr, cv::COLOR_RGB2BGR);
        return r_bgr;
    }
    else if (f.get_profile().format() == RS2_FORMAT_Z16)
    {
        return {cv::Size(w, h), CV_16UC1, (void*)f.get_data(), cv::Mat::AUTO_STEP};
    }
    else if (f.get_profile().format() == RS2_FORMAT_Y8)
    {
        return {cv::Size(w, h), CV_8UC1, (void*)f.get_data(), cv::Mat::AUTO_STEP};
    }
    else if (f.get_profile().format() == RS2_FORMAT_DISPARITY32)
    {
        return {cv::Size(w, h), CV_32FC1, (void*)f.get_data(), cv::Mat::AUTO_STEP};
    }

    throw std::runtime_error("Frame format is not supported yet!");
}