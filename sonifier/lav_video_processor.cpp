
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
    else
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



void lavVideoProcessor::acquireAndProcessFrame() {

    cv::Mat img = cv::Mat(sizeColor, CV_8UC1);
    _inputMatColor = _capture->getNextFrameColor();
    _inputMat = _capture->getNextFrame();
    cv::cvtColor(_inputMatColor, img, cv::COLOR_BGR2GRAY);
    stagDetector.detectMarkers(img);
    Drawer::drawMarkers(&img, stagDetector.markers);
    DataVideoProcessing data;
    cv::GaussianBlur(_inputMat, _inputMat, cv::Size(3,3),1);
    for(auto const& mrk: stagDetector.markers)
    {
        short distance = _inputMat.at<unsigned short>((int)mrk.center.x, (int)mrk.center.y);
        std::cout<<distance<<std::endl;
        data.data_path.push_back({(unsigned int)mrk.center.x, (unsigned int)mrk.center.y, distance, mrk.id});

    }
    push_data(data);
    cv::imshow("image", img);
    cv::waitKey(1);
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






