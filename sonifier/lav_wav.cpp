
#include "lav_wav.h"

int lavWav::loadWav(float*** sound_db, char* filename) {

    std::unique_ptr<SoundWav> wavfile(new SoundWav(filename));
    if(wavfile->readHeader())
    {
        printf("Unable to read the file's header");
    }

    char str[200];
    sprintf(str, "Trying to load database %s\n", filename);
    lavLog::LAVLOG(str);


    FILE* wav_file = fopen(filename, "rb");


    fseek(wav_file, wavfile->getSizeHeader(), SEEK_SET);

    for (int ID_x =0; ID_x<FRAME_WIDTH_SONIFIED; ++ID_x) {

        sound_db[ID_x] = (float**) calloc(FRAME_HEIGHT_SONIFIED, sizeof(float*));
        float** curr_sound_db_x = sound_db[ID_x];

        for (int ID_y =0; ID_y<FRAME_HEIGHT_SONIFIED; ++ID_y) {
            curr_sound_db_x[ID_y] = (float*) calloc(SIZE_SOUND_IN_VALUE, sizeof(float));
            float* curr_sound_db_x_y = curr_sound_db_x[ID_y];
            fread(curr_sound_db_x_y, sizeof(float), SIZE_SOUND_IN_VALUE, wav_file);
            //to use with RK3188 and not for RK3288 ???
            //fseek(_wav_file, SIZE_SOUND_IN_VALUE, SEEK_CUR);

        }
    }

	return 0;

}


