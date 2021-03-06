
#include "lav_video_processor.h"



cv::Size sizeDepth(DEPTH_FRAME_WIDTH, DEPTH_FRAME_WIDTH);//SHORT_RANGE
cv::Size sizeColor(COLOR_FRAME_WIDTH, COLOR_FRAME_WIDTH);//SHORT_RANGE

cv::Mat lavVideoProcessor::_inputMat;

cv::Mat lavVideoProcessor::_inputMatColor;

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

float lavVideoProcessor::ratioWidth = COLOR_FRAME_WIDTH / WIDTH;
float lavVideoProcessor::ratioHeight = COLOR_FRAME_HEIGHT / HEIGHT;

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
    _newValue = false;
    _silence = true;
}
#endif

void lavVideoProcessor::release() {

	_close_video = 1;

}

void lavVideoProcessor::start()
{
    pthread_mutex_lock(&mutex_data_out);
    _newValue = false;
    _silence = false;
    pthread_mutex_unlock(&mutex_data_out);
}

void lavVideoProcessor::stop()
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
        usleep(250);
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


void lavVideoProcessor::acquireAndProcessFrame() {


    _inputMat = _capture->getNextFrame();
    cv::Mat img = cv::Mat(sizeColor, CV_8UC1);
    _inputMatColor = _capture->getNextFrameColor();
    //Drawer draw;
    cv::cvtColor(_inputMatColor, img, cv::COLOR_BGR2GRAY);
    stagDetector.detectMarkers(img);

    //cv::imwrite("/home/ubuntu/CLionProjects/SoundPathFinding/frame_color.png", _inputMatColor);
    //draw.drawMarkers(&_inputMatColor, stagDetector.markers);
    //cv::imwrite("/home/ubuntu/CLionProjects/SoundPathFinding/img_marker.png", _inputMatColor);
    cv::Mat _threshMat = cv::Mat(cv::Size(_inputMat.cols ,_inputMat.cols), CV_16UC1);
    cv::Mat _threshMatH = cv::Mat(cv::Size(_inputMat.cols ,_inputMat.cols), CV_16UC1);;
    cv::Mat _threshMatL= cv::Mat(cv::Size(_inputMat.cols ,_inputMat.cols), CV_16UC1);
    cv::threshold(_inputMat, _threshMatH, 100, 1, cv::THRESH_BINARY);

    cv::threshold(_inputMat, _threshMatL, 1300, 1, cv::THRESH_BINARY_INV);
    _threshMat = (_threshMatH & _threshMatL) * 255;
    _threshMat.convertTo(_threshMat, CV_8UC1, 1);

    //Drawer::drawMarkers(&img, stagDetector.markers);
    DataVideoProcessing data;




    for(auto const& mrk: stagDetector.markers)
    {
        int angle = lavVideoCapture::pixelToAng((int)(mrk.center.x), FOV_X, COLOR_FRAME_WIDTH)  + (int)(FOV_X / 2);
        unsigned short distance = _inputMat.at<unsigned short>(mrk.center.y, mrk.center.x);
        data.data_path.push_back({(unsigned int)mrk.center.x, (unsigned int)mrk.center.y, distance, angle ,mrk.id});
        //std::cout<<distance<< " "<< mrk.id << std::endl;
    }
    for(int i = 0; i < _inputMat.cols * _inputMat.rows; i++)
    {
        if(_inputMat.at<unsigned short>(i) < 100 || _inputMat.at<unsigned short>(i) > 10000)
        {
            _inputMat.at<unsigned short>(i) = 0;
        }
        else
           _inputMat.at<unsigned short>(i) = _inputMat.at<unsigned short>(i) * 5;
    }
    //cv::imwrite("/home/ubuntu/CLionProjects/SoundPathFinding/threshed.png", _threshMat);
    //cv::imwrite("/home/ubuntu/CLionProjects/SoundPathFinding/depthmap.png", _inputMat);

    //cv::imshow("ddd", _threshMat);
    //cv::waitKey(1);
    cv::resize(_threshMat, data.sonify, cv::Size(FRAME_WIDTH_SONIFIED, FRAME_HEIGHT_SONIFIED));
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






