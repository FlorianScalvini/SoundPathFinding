
#include "lav_video_processor.h"



cv::Size sizeDepth(DEPTH_FRAME_WIDTH, DEPTH_FRAME_WIDTH);//SHORT_RANGE
cv::Size sizeColor(COLOR_FRAME_WIDTH, COLOR_FRAME_WIDTH);//SHORT_RANGE


//cv::Size size(160, 120);//LONG_RANGE
bool lavVideoProcessor::_colorProcessingDone = false;
bool lavVideoProcessor::_colorProcessingReady = false;
pthread_mutex_t lavVideoProcessor::_mutexColorProcessing = PTHREAD_MUTEX_INITIALIZER;
cv::VideoWriter* lavVideoProcessor::_pVideoWriter = new  cv::VideoWriter("recording.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, sizeDepth, true);
unsigned int lavVideoProcessor::_cptVideoFrame = 0;
cv::Mat lavVideoProcessor::_inputMat;
cv::Mat lavVideoProcessor::_inputMatScaled = cv::Mat(sizeDepth, CV_8UC1);
cv::Mat lavVideoProcessor::_inputMatColor;
cv::Mat lavVideoProcessor::_tmpMat;
cv::Mat lavVideoProcessor::_outputMat;
cv::Mat lavVideoProcessor::_mOutputRGBA;
cv::Mat lavVideoProcessor::_previousMat;
cv::Mat lavVideoProcessor::_outputMatForDisplay;
cv::Mat lavVideoProcessor::_colorFrameForVideoRecording = cv::Mat(sizeDepth, CV_8UC3);

lavVideoCapture* lavVideoProcessor::_capture = 0;

bool lavVideoProcessor::_silence = 0;
bool lavVideoProcessor::_firstFrame = true;
int lavVideoProcessor::_nbNonZero = 0;
int lavVideoProcessor::_close_video = 0;

//pthread_t lavVideoProcessor::thread_video_processing;


#ifdef DEPTH_VIDEOPROCESSING
cv::Mat lavVideoProcessor::currentGoodPixelsMask = cv::Mat(sizeDepth, CV_8UC1);
cv::Mat lavVideoProcessor::previousGoodPixelsMask = cv::Mat(sizeDepth, CV_8UC1);
cv::Mat lavVideoProcessor::currentAndPreviousGoodPixelsMask = cv::Mat(sizeDepth, CV_8UC1);
cv::Mat lavVideoProcessor::newGoodPixelsMask = cv::Mat(sizeDepth, CV_8UC1);

cv::Mat lavVideoProcessor::depthProximityAlarmMask = cv::Mat(sizeDepth, CV_8UC1);
cv::Mat lavVideoProcessor::frameDifferencing = cv::Mat(sizeDepth, CV_8UC1);
cv::Mat lavVideoProcessor::frameDifferencingMask = cv::Mat(sizeDepth, CV_8UC1);
cv::Mat lavVideoProcessor::selectedFrameDifferencing = cv::Mat(sizeDepth, CV_8UC1);
cv::Mat lavVideoProcessor::finalMaskDepth = cv::Mat(sizeDepth, CV_8UC1);
#endif

#ifdef OBJECT_DETECTION
std::map<unsigned int, std::vector<ObjectBoundingBox>> lavVideoProcessor::_detOutput = {};
std::list<unsigned int> lavVideoProcessor::classes = {0, 1, 2, 3, 5, 7};

Detecteur*  lavVideoProcessor::_detecteur = 0;
pthread_mutex_t lavVideoProcessor::_mutexInference = PTHREAD_MUTEX_INITIALIZER;
cv::Scalar colors[256];
Sort lavVideoProcessor::_sort;
#endif

#ifdef PATH_MARKER
Stag lavVideoProcessor::stagDetector = Stag(15, 7, false);
#endif



#ifdef OBJECT_DETECTION
void lavVideoProcessor::init() {
    //_detecteur = new DetecteurLibtorch("/home/ubuntu/CLionProjects/sonifier/Detection/yolov5/yolov5s.torchscript.pt");
    _detecteur = new DetecteurTensorRT("/home/ubuntu/Bureau/sonifier/Detection/yolov5/yolov5s.engine");
    lavVideoProcessor::_capture = lavRealsenseCamera::getSingleton();
    _mOutputRGBA = cv::Mat(FRAME_HEIGHT_SONIFIED, FRAME_WIDTH_SONIFIED, CV_8UC4);
    _sort = Sort(5,0.3);
    for(int i = 0; i < 256; i++)
    {
        colors[i] = cv::Scalar(rand() % 0xFF, rand() % 0xFF, rand() % 0xFF);
    }
}
#else
void lavVideoProcessor::init() {
    lavVideoProcessor::_capture = lavRealsenseCamera::getSingleton();
    _mOutputRGBA = cv::Mat(FRAME_HEIGHT_SONIFIED, FRAME_WIDTH_SONIFIED, CV_8UC4);
}
#endif

void lavVideoProcessor::release() {

	_close_video = 1;

}

void lavVideoProcessor::startOrStopSound() {
	if (!_silence) {
		_silence = true;
	}
	else {
		_silence = false;
	}
}

#ifdef DEPTH_VIDEOPROCESSING
void lavVideoProcessor::processFrame() {


    for(int j = 0; j < _inputMat.rows; j++)
    {
        for( int i = 0; i < _inputMat.cols; i++)
        {
            if(_inputMatScaled.at<unsigned char>(j,i) == 255
               || _previousMat.at<unsigned char>(j,i) == 255
               || _inputMatScaled.at<unsigned char>(j,i) == 0)
                frameDifferencingMask.at<unsigned char>(j,i) = 0;
            else
                frameDifferencingMask.at<unsigned char>(j,i) = abs(_inputMatScaled.at<unsigned char>(j,i) - _previousMat.at<unsigned char>(j,i));
        }
    }

    _inputMatScaled.copyTo(_previousMat);
    cv::GaussianBlur(frameDifferencingMask, frameDifferencingMask, cv::Size(3,3), 2.0, 2.0);
    cv::threshold(frameDifferencingMask, frameDifferencingMask, MIN_GRAYSCALE_SONIFICATION, 1, cv::THRESH_BINARY);

    cv::threshold(_inputMatScaled, depthProximityAlarmMask, 255, 1, cv::THRESH_BINARY);

    //finalMaskDepth = frameDifferencingMask+depthProximityAlarmMask;
    finalMaskDepth = frameDifferencingMask;
    cv::multiply(_inputMatScaled, finalMaskDepth, _outputMat);



    //cv::threshold(_inputMat, _inputMatThreshold,254,0,cv::THRESH_TOZERO_INV);




    /*//Sans Filtre
    cv::resize(_inputMat, _outputMat, cv::Size(FRAME_WIDTH_SONIFIED, FRAME_HEIGHT_SONIFIED), 0, 0, 0);
    */
}

#else
void lavVideoProcessor::processFrame() {



	//to display input
	//_inputMat.copyTo(_outputMatForDisplay);


	//lavConstants::__startTimeChecking();

	cv::absdiff(_inputMat, _previousMat, _outputMat);
	_inputMat.copyTo(_previousMat);
	cv::GaussianBlur(_outputMat, _outputMat, cv::Size(3,3), 2.0, 2.0);
	cv::threshold(_outputMat, _outputMat, 100, 255, 0);


	//lavConstants::__stopTimeChecking("standard video processing");

	/*int nbActivePixel = cv::countNonZero(_outputMat);
	if (nbActivePixel>10) {
		_outputMat.setTo(cv::Scalar(255));
	}*/

	//_outputMat.setTo(cv::Scalar(255));
	/*int x = 140;
	int y = 60;
	cv::rectangle( _outputMat, cv::Point( x, y), cv::Point(x+10, y+10), cv::Scalar( 255), -1);*/

	//to comment if no display on the screen
	//cv::cvtColor(_outputMat, _mOutputRGBA, 9);

}
#endif


void lavVideoProcessor::acquireAndProcessFrame() {

    pthread_mutex_lock(&_mutexColorProcessing);
    _colorProcessingReady = true;
    pthread_mutex_unlock(&_mutexColorProcessing);
    _inputMat = _capture->getNextFrame();

/*
    for(int j = 0; j < _inputMat.rows; j++)
    {
        for( int i = 0; i < _inputMat.cols; i++)
        {
            if(_inputMat.at<unsigned short>(j,i) == 0)
            {
                _inputMatScaled.at<unsigned char>(j,i) = 255;
            }
            else if(_inputMat.at<unsigned short>(j,i) > 5000 || _inputMat.at<unsigned short>(j,i) < 29)
            {
                _inputMatScaled.at<unsigned char>(j,i) = 0;
            }
            else
            {
                _inputMatScaled.at<unsigned char>(j,i) = (unsigned char)(255 - 0.051 * _inputMat.at<unsigned short>(j,i));
            }
        }
    }

    if (_firstFrame) {
        _firstFrame = false;
        _previousMat = _inputMatScaled.clone();
        _outputMat = _inputMatScaled.clone();
        _outputMatForDisplay = _inputMatScaled.clone();
    }
    if (! _silence) {
        processFrame();
    }
    else {
        _outputMat.setTo(cv::Scalar(0));
    }

    if (_firstFrame) {
        _outputMat.setTo(cv::Scalar(0));
    }


     cv::imshow("COLOR THREAD", _inputMatColor);
     cv::imshow("OUTPUT ", _outputMat);
     _inputMatScaled.copyTo(_previousMat);
     //_previousDetOutput = _detOutput;
     cv::resize(_outputMat, _outputMat, cv::Size(FRAME_WIDTH_SONIFIED, FRAME_HEIGHT_SONIFIED), 0, 0, 0);

     lavSonifier::sonify(&_outputMat);
     cv::waitKey(1);
    */
    while (!_colorProcessingDone) {
        usleep(500);
    }
    pthread_mutex_lock(&_mutexColorProcessing);
    _colorProcessingDone = false;
    _colorProcessingReady = true;
    pthread_mutex_unlock(&_mutexColorProcessing);
}

#ifdef PATH_MARKER
void* lavVideoProcessor::acquireAndProcessFrameColor(void *args)
{
    cv::Mat img = cv::Mat(sizeColor, CV_8UC1);
    while(!_close_video)
    {
        while (!_colorProcessingReady) {
            usleep(500);
        }
        pthread_mutex_lock(&_mutexColorProcessing);
        _colorProcessingReady = false;
        pthread_mutex_unlock(&_mutexColorProcessing);
        _inputMatColor = _capture->getNextFrameColor();
        cv::cvtColor(_inputMatColor, img, cv::COLOR_BGR2GRAY);
        stagDetector.detectMarkers(img);


        pthread_mutex_lock(&_mutexColorProcessing);
        _colorProcessingDone = true;
        pthread_mutex_unlock(&_mutexColorProcessing);
    }
    return nullptr;
}
#else

void* lavVideoProcessor::acquireAndProcessFrameColor(void *args)
{
    while(!_close_video)
    {
        while (!_colorProcessingReady) {
            usleep(500);
        }
        pthread_mutex_lock(&_mutexColorProcessing);

        _colorProcessingReady = false;
        pthread_mutex_unlock(&_mutexColorProcessing);
        _inputMatColor = _capture->getNextFrameColor();
        pthread_mutex_lock(&_mutexColorProcessing);
        _colorProcessingDone = true;
        pthread_mutex_unlock(&_mutexColorProcessing);
    }
    return nullptr;
}
#endif

void* lavVideoProcessor::start_video_stream(void* args) {
	//int i = 0;
    //auto t_start = std::chrono::high_resolution_clock::now();
    pthread_t color_thread;
    pthread_create(&color_thread, nullptr, acquireAndProcessFrameColor, (void*)nullptr);
    while (! _close_video) {
		acquireAndProcessFrame();
        /*
		//usleep(5000);
        i++;
        if(i%30 == 0)
        {
            auto t_end = std::chrono::high_resolution_clock::now();
            double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();
            std::cout<<elapsed_time_ms / 30.0<<std::endl;
            t_start = std::chrono::high_resolution_clock::now();
            i = 0;
        }*/
	}
	lavLog::LAVLOG("video_close\n");
	//_pCam->closeAcquisition();
	_capture->release();


	return nullptr;
}


void lavVideoProcessor::start_thread_video_stream() {
    //usleep(10000);
	pthread_t thread_video_processing;
	pthread_create(&thread_video_processing, nullptr, start_video_stream, (void*)nullptr);
}

