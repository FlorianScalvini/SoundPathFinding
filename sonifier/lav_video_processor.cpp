
#include "lav_video_processor.h"



cv::Size sizeDepth(DEPTH_FRAME_WIDTH, DEPTH_FRAME_WIDTH);//SHORT_RANGE
cv::Size sizeColor(COLOR_FRAME_WIDTH, COLOR_FRAME_WIDTH);//SHORT_RANGE

cv::Mat lavVideoProcessor::_inputMat;

cv::Mat lavVideoProcessor::_inputMatColor;
cv::Mat lavVideoProcessor::_tmpMat;
cv::Mat lavVideoProcessor::_outputMat;
cv::Mat lavVideoProcessor::_mOutputRGBA;
cv::Mat lavVideoProcessor::_previousMat;
cv::Mat lavVideoProcessor::_outputMatForDisplay;
DataVideoProcessing lavVideoProcessor::transData;

cv::Size size(320, 240);//SHORT_RANGE

#ifdef DEPTH_VIDEOPROCESSING
cv::Mat lavVideoProcessor::currentGoodPixelsMask = cv::Mat(size, CV_8UC1);
cv::Mat lavVideoProcessor::previousGoodPixelsMask = cv::Mat(size, CV_8UC1);
cv::Mat lavVideoProcessor::currentAndPreviousGoodPixelsMask = cv::Mat(size, CV_8UC1);
cv::Mat lavVideoProcessor::newGoodPixelsMask = cv::Mat(size, CV_8UC1);
cv::Mat lavVideoProcessor::depthProximityAlarmMask = cv::Mat(size, CV_8UC1);
cv::Mat lavVideoProcessor::frameDifferencingMask = cv::Mat(size, CV_8UC1);
cv::Mat lavVideoProcessor::selectedFrameDifferencing = cv::Mat(size, CV_8UC1);
cv::Mat lavVideoProcessor::finalMaskDepth = cv::Mat(size, CV_8UC1);
#endif

bool lavVideoProcessor::_newValue;
lavVideoCapture* lavVideoProcessor::_capture = nullptr;

bool lavVideoProcessor::_silence = false;
bool lavVideoProcessor::_firstFrame = true;
int lavVideoProcessor::_close_video = 0;
pthread_mutex_t lavVideoProcessor::mutex_data_out = PTHREAD_MUTEX_INITIALIZER;
//pthread_t lavVideoProcessor::thread_video_processing;


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
bool lavVideoProcessor::performStag;
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
    _newValue = false;
    _silence = true;
}
#endif

void lavVideoProcessor::release() {

	_close_video = 1;

}

void lavVideoProcessor::startSound()
{
    pthread_mutex_lock(&mutex_data_out);
    _newValue = false;
    _silence = false;
    pthread_mutex_unlock(&mutex_data_out);
}

void lavVideoProcessor::stopSound()
{
    pthread_mutex_lock(&mutex_data_out);
    _newValue = false;
    _silence = true;
    pthread_mutex_unlock(&mutex_data_out);
}

void lavVideoProcessor::startOrStopSound() {
    pthread_mutex_lock(&mutex_data_out);
    _newValue = false;
    pthread_mutex_unlock(&mutex_data_out);
	if (!_silence) {
		_silence = true;
	}
	else {
		_silence = false;
	}
}

DataVideoProcessing lavVideoProcessor::pull_data()
{
    DataVideoProcessing dataOut;
    while(!_newValue)
        usleep(500);
    pthread_mutex_lock(&mutex_data_out);
    if(!_silence)
        dataOut = transData;
    _newValue = false;
    pthread_mutex_unlock(&mutex_data_out);
    return dataOut;
}

void lavVideoProcessor::push_data(DataVideoProcessing data) {
    pthread_mutex_lock(&mutex_data_out);
    transData = data;
    _newValue = true;
    pthread_mutex_unlock(&mutex_data_out);
}


void lavVideoProcessor::frameDifferencing()
{
    if (_firstFrame) {
        _firstFrame = false;
        _previousMat = _inputMat.clone();
        _outputMat = _inputMat.clone();
        _outputMatForDisplay = _inputMat.clone();
    }
    for(int j = 0; j < _inputMat.rows; j++)
    {
        for( int i = 0; i < _inputMat.cols; i++)
        {
            if(_inputMat.at<unsigned char>(j,i) == 255
               || _previousMat.at<unsigned char>(j,i) == 255
               || _inputMat.at<unsigned char>(j,i) == 0)
                frameDifferencingMask.at<unsigned char>(j,i) = 0;
            else
                frameDifferencingMask.at<unsigned char>(j,i) = abs(_inputMat.at<unsigned char>(j,i) - _previousMat.at<unsigned char>(j,i));
        }
    }
    _inputMat.copyTo(_previousMat);
    cv::GaussianBlur(frameDifferencingMask, frameDifferencingMask, cv::Size(3,3), 2.0, 2.0);
    cv::threshold(frameDifferencingMask, frameDifferencingMask, MIN_GRAYSCALE_SONIFICATION, 1, cv::THRESH_BINARY);
    cv::threshold(_inputMat, depthProximityAlarmMask, 215, 1, cv::THRESH_BINARY);
    finalMaskDepth = frameDifferencingMask+depthProximityAlarmMask;
    cv::multiply(frameDifferencingMask, finalMaskDepth, _outputMat);
    cv::resize(_outputMat, _outputMat, cv::Size(FRAME_WIDTH_SONIFIED, FRAME_HEIGHT_SONIFIED), 0, 0, 0);
}

void lavVideoProcessor::acquireAndProcessFrame() {

    _inputMat = _capture->getNextFrame();
    frameDifferencing();
    _outputMat.setTo(cv::Scalar(0));

    cv::Mat img = cv::Mat(sizeColor, CV_8UC1);
    _inputMatColor = _capture->getNextFrameColor();
    cv::cvtColor(_inputMatColor, img, cv::COLOR_BGR2GRAY);
    stagDetector.detectMarkers(img);
    Drawer::drawMarkers(&img, stagDetector.markers);
    DataVideoProcessing data;
    //cv::GaussianBlur(_inputMat, _inputMat, cv::Size(3,3),1);



    for(auto const& mrk: stagDetector.markers)
    {
        ////// ATTT
        unsigned short distance = _inputMat.at<unsigned short>((int)mrk.center.y, (int)mrk.center.x);
        data.data_path.push_back({(unsigned int)mrk.center.x, (unsigned int)mrk.center.y, distance, mrk.id});

    }

    /*
    int rangeSector = 320 / 5;
    for(int i = 0; i < 320; i++)
    {
        for(int j = 0; j < 160; j++)
        {
            if(imgResize.at<unsigned short>(j,i) < 5000 && imgResize.at<unsigned short>(j,i) > 400)
            {
                data.sector[i/rangeSector]++;
            }
        }
    }*/
    push_data(data);
}

void* lavVideoProcessor::start_video_stream(void* args) {
    //int i = 0;
    //auto t_start = std::chrono::high_resolution_clock::now();;
    while (! _close_video) {
        if(!_silence)
            acquireAndProcessFrame();
        else
            usleep(1000);
    }
    lavLog::LAVLOG("video_close\n");
    _capture->release();
    return nullptr;
}


void lavVideoProcessor::start_thread_video_stream() {
    //usleep(10000);
    pthread_t thread_video_processing;
    pthread_create(&thread_video_processing, nullptr, start_video_stream, (void*)nullptr);
}






