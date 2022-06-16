#ifndef LAV_OPENCV_CAMERA
#define LAV_OPENCV_CAMERA

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>
//#include <opencv2/ocl/ocl.hpp>

#include "lav_log.h"
#include "lav_constants.h"
#include "lav_video_capture.h"

class lavOpencvCamera: public lavVideoCapture
{

    private:

		cv::Mat _inputMat;
		cv::Mat _tmpMat;

        cv::VideoCapture* _capture;

    public:

        lavOpencvCamera(int IDCamera = 0);

        cv::Mat getNextFrame();
        void release();

};

#endif
