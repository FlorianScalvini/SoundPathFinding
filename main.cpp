#include "sonifier/lav.h"
#include <unistd.h>

int main() {
    FILE *_wav_file = fopen("/home/ubuntu/Bureau/HrtfConvo/person.wav", "rb");
    if (!_wav_file) {
        char str[100];
        sprintf(str, "Unable to load file %s\n", "/home/ubuntu/Bureau/HrtfConvo/person.wav");
        std::cout<<str<<std::endl;
    }

    fseek(_wav_file, 78, SEEK_SET);
    float* data[100000];
    fread(data, sizeof(short), 14978, _wav_file);

    fclose(_wav_file);


#ifdef DESKTOP
    //lav::setDefaultDatabasePath("/home/ubuntu/data/boulot/recherche/lead/development/C++/sonifier/sonifier/res/HRTF_SCUT020_180H_90V.wav");
    lav::setDefaultDatabasePath("/home/ubuntu/CLionProjects/sonifier/res/lav_default_sound_db.wav");
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

