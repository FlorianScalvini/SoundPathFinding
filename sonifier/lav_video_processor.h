
#ifndef LAV_VIDEO_PROCESSOR
#define LAV_VIDEO_PROCESSOR

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/ocl/ocl.hpp>



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



class lavVideoProcessor {

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




        static unsigned int _cptVideoFrame;
        static cv::Mat _inputMat;
        static cv::Mat _inputMatScaled;
    static cv::Mat _tmpMat;
		static cv::Mat _outputMat;
		static cv::Mat _mOutputRGBA;
		static cv::Mat _previousMat;
		static cv::Mat _outputMatForDisplay;
        static cv::Mat _colorFrameForVideoRecording;


        #ifdef DEPTH_VIDEOPROCESSING
        static cv::Mat previousGoodPixelsMask;
        static cv::Mat currentGoodPixelsMask;
        static cv::Mat currentAndPreviousGoodPixelsMask;
        static cv::Mat newGoodPixelsMask;
        static cv::Mat depthProximityAlarmMask;
        static cv::Mat frameDifferencing;
        static cv::Mat frameDifferencingMask;
        static cv::Mat selectedFrameDifferencing;
        static cv::Mat finalMaskDepth;
        #endif




        //static pthread_t thread_video_processing;

		//for LavCamera
		//LavCamera* _pCam;
		//end for LavCamera

		//for android Camera
    	//static cv::VideoCapture* _capture;
    	static lavVideoCapture* _capture;

        static cv::VideoWriter* _pVideoWriter;

        //static lavVideoCapture* _lavVideoCapture;
    	//end android Camera

    	static bool _silence;

        static bool _firstFrame;
        static int _nbNonZero;

        static int _close_video;
    static bool _colorProcessingReady;
        static pthread_mutex_t _mutexColorProcessing;
    private:

	public:

	    static void init();
	    static void release();
	    static void startOrStopSound();
	    static void processFrame();
	    static void acquireAndProcessFrame();
	    static void* start_video_stream(void* args);
        static void* acquireAndProcessFrameColor(void *args);
	    static void start_thread_video_stream();

    static bool _colorProcessingDone;
    static cv::Mat _inputMatColor;
};


#endif
