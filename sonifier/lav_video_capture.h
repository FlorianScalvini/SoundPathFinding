#ifndef LAV_VIDEO_CAPTURE
#define LAV_VIDEO_CAPTURE

//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/videoio.hpp>

class lavVideoCapture {

    public:

        virtual cv::Mat getNextFrame() = 0;
        virtual cv::Mat getNextFrameColor() = 0;
        virtual void release() = 0;
};


#endif
