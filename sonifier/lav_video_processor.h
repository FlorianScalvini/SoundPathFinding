
#ifndef LAV_VIDEO_PROCESSOR
#define LAV_VIDEO_PROCESSOR

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/ocl/ocl.hpp>
#include "Drawer.h"
#include "lav_manager.h"

//#include <android_native_app_glue.h>

#include "lav_log.h"
#include "lav_sonifier.h"
#include "lav_audio_stream.h"
#include "lav_video_capture.h"
#include "lav_opencv_camera.h"
#ifdef DESKTOP
#include "lav_opencv_videofile.h"
#endif
#include "lav_udp_camera.h"
//#include "lav_softkinetic_camera.h"
#include "lav_camera.h"
#include "lav_realsense_camera.h"
#include <list>

#ifdef OBJECT_DETECTION
#include "Detection/DetecteurLibtorch.h"
#include "Detection/Detecteur.h"
#include "Detection/DetecteurTensorRT.h"
#endif

#include "lav_manager.h"
#include "Stag.h"


struct PathOut{
    unsigned int x_pixel;
    unsigned int y_pixel;
    int z_grayscale;
    int label_i;
};

struct DataVideoProcessing
{
    std::vector<PathOut> data_path;
};




class lavVideoProcessor {
public:
    private:
        #ifdef OBJECT_DETECTION
        static std::map<unsigned int, std::vector<ObjectBoundingBox>> _detOutput ;
        static std::list<unsigned int> classes;
        static pthread_mutex_t _mutexInference;
        static bool _inferenceDone;
        static bool _readyNewInference;
        static Sort _sort;

        static Detecteur* _detecteur;
        #endif


        static cv::Mat _inputMat;
        static cv::Mat _inputMatColor;
        static cv::Mat _tmpMat;
		static cv::Mat _outputMat;
		static cv::Mat _mOutputRGBA;
		static cv::Mat _previousMat;
		static cv::Mat _outputMatForDisplay;
        static DataVideoProcessing transData;

        static Stag stagDetector;
    	static lavVideoCapture* _capture;
    	static bool _silence;
        static bool _firstFrame;
        static int _close_video;

	public:

	    static void init();
	    static void release();
	    static void startOrStopSound();
	    static void processFrame();
	    static void acquireAndProcessFrame();
        static void push_data(DataVideoProcessing data);
        static DataVideoProcessing pull_data();
	    static void* start_video_stream(void* args);
        static void start_thread_video_stream();
        static pthread_mutex_t mutex_data_out;
        static bool newValue;
};

#endif