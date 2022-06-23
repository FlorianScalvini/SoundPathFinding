#include "sonifier/lav.h"
#include <unistd.h>

int main() {


#ifdef DESKTOP
    //lav::setDefaultDatabasePath("/home/maxime/data/boulot/recherche/lead/development/C++/sonifier/sonifier/res/HRTF_SCUT020_180H_90V.wav");
    lav::setDefaultDatabasePath("/home/florian/CLionProjects/sonifier/res/lav_default_sound_db.wav");
#else
    lav::setDefaultDatabasePath("/home/florian/CLionProjects/sonifier/res/lav_default_sound_db.wav");
    //lav::setDefaultDatabasePath("/home/florian/CLionProjects/sonifier/res/HRTF_SCUT020_180H_90V.wav");
#endif
    lav::start();

    getchar();
    lav::stop();
    return 0;
}
