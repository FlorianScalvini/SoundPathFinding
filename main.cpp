#include "sonifier/lav.h"
#include <unistd.h>
#include "Stag.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
/*
    cv::Mat _inputMatColor = cv::imread("/home/ubuntu/CLionProjects/SoundPathFinding/5 distorted quads.png", cv::IMREAD_COLOR);
    cv::Mat img;
    Stag stagDetector = Stag(15, 7, false);

    cv::cvtColor(_inputMatColor, img, cv::COLOR_BGR2GRAY);
    stagDetector.detectMarkers(img);
    Drawer::drawMarkers(&img, stagDetector.markers);
    stagDetector.logResults("/home/ubuntu/CLionProjects/SoundPathFinding/");
    std::cout<<"";

*/


#ifdef DESKTOP
    //lav::setDefaultDatabasePath("/home/florian/data/boulot/recherche/lead/development/C++/sonifier/sonifier/res/HRTF_SCUT020_180H_90V.wav");
    lav::setDefaultDatabasePath("/home/florian/CLionProjects/sonifier/res/lav_default_sound_db.wav");
#else
    lav::setDefaultDatabasePath("/home/ubuntu/CLionProjects/sonifier/res/lav_default_sound_db.wav");
    //lav::setDefaultDatabasePath("/home/ubuntu/ubuntu/sonifier/res/HRTF_SCUT020_180H_90V.wav");
#endif
    lav::start();
    getchar();
    lav::stop();

    //while (1) {
    //    usleep(500000); //0.5s
    //}

}

