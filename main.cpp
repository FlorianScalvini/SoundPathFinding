#include "sonifier/lav.h"
#include <unistd.h>

int main() {
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

