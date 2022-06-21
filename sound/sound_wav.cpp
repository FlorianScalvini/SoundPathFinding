//
// Created by ubuntu on 20/06/22.
//

#include "sound_wav.h"

SoundWav::SoundWav(char *filepath) : filename(filepath)
{
    _wav_file = nullptr;
}

int SoundWav::getSizeData(){
    this->sizeData;
}

int SoundWav::getSizeHeader(){
    this->sizeHeader;
}

int SoundWav::readHeader()
{
    if(_wav_file == nullptr)
        fclose(_wav_file);
    _wav_file = fopen(filename, "rb");
    if (!_wav_file) {
        char str[100];
        sprintf(str, "Unable to load file %s\n", filename);
        std::cout<<str<<std::endl;
        return EXIT_FAILURE;
    }

    fseek(_wav_file, 0, SEEK_SET);
    unsigned char buffer[HEADER_SIZE]; // Headers size
    unsigned char* ptr = buffer;
    int i = 0;
    int index = 0;
    while(i < HEADER_SIZE)
    {
        fread(buffer+i, sizeof(unsigned char), 1, _wav_file);
        index++;
        if((i == 12 && buffer[12] != 0x66) || (i == 13 && buffer[13] != 0x6D) || (i == 14 && buffer[14] != 0x74) ||( i == 15 && buffer[15] != 0x20))
        {
            i = 12;
            continue;
        }
        if((i == 36 && buffer[36] != 0x64) || (i == 37 && buffer[37] != 0x61) || (i == 38 && buffer[38] != 0x74) || (i == 39 && buffer[39] != 0x61))
        {
            i = 36;
            continue;
        }
        if(buffer[i] == EOF)
        {
            std::cout<<"Error : EOF value"<<std::endl;
            break;
        }
        ptr++;
        i++;
    }
    fclose(_wav_file);
    sizeHeader = index;
    sizeData =  8 * header.data_sz  / (header.bits_per_sample);
    memcpy(&header ,buffer, HEADER_SIZE);
    return EXIT_SUCCESS;
}


int SoundWav::writeStandardWavFile(const char* filename, char* format, char* data, unsigned int nbByte) {

    //int nbSample = 10000;
    //short* sound = (short*) calloc(nbSample*2, sizeof(short));
    int nbBytePerSample = 0;
    int nbBitPerValue = 0;
    int uFormat = 0;
    int nbSample = 0;

    if (strcmp(format, "FLOAT")==0) {
        nbSample = nbByte/(2*sizeof(float));
        nbBytePerSample = sizeof(float)*2;
        nbBitPerValue = sizeof(float)*8;
        uFormat = FORMAT_FLOAT;

    }
    else {
        nbSample = nbByte/(2*sizeof(short));
        nbBytePerSample = sizeof(short)*2;
        nbBitPerValue = sizeof(short)*8;
        uFormat = FORMAT_PCM;
    }



    FILE* wavfile = nullptr;
    Wav_header wavheader;

    wavheader.riff_id = ID_RIFF;
    wavheader.riff_sz = 0;

    wavheader.wave_id = ID_WAVE;

    wavheader.fmt_id = ID_INFO;
    wavheader.fmt_sz = 12;
    wavheader.audio_format = uFormat;
    wavheader.num_channels = 2;
    wavheader.sample_rate = 44100;
    wavheader.bits_per_sample = nbBitPerValue;
    wavheader.byte_rate = (wavheader.bits_per_sample / nbBytePerSample) * wavheader.num_channels * wavheader.sample_rate;
    wavheader.block_align = wavheader.num_channels * (wavheader.bits_per_sample / nbBytePerSample);
    wavheader.data_id = ID_DATA;

    wavfile = fopen(filename, "wb");
    if (!wavfile) {
        char str[100];
        sprintf(str, "Unable to create file %s\n", filename);
        printf(str);
        return 1;
    }

    fseek(wavfile, sizeof(struct Wav_header), SEEK_SET);


    //if (fwrite((void*)sound, 1, nbByte, wav_file) != nbByte) {
    if (fwrite(data, 1, nbByte, wavfile) != nbByte) {
        printf("Error writing recorded data to Wav file\n");
        return 0;
    }

    int copied_nb_frames = nbByte/nbBytePerSample;

    wavheader.data_sz = copied_nb_frames * wavheader.block_align;
    wavheader.riff_sz = wavheader.data_sz + sizeof(wavheader) - 8;
    fseek(wavfile, 0, SEEK_SET);
    fwrite(&wavheader, sizeof(struct Wav_header), 1, wavfile);
    fclose(wavfile);

    return 1;

}

Wav_header* SoundWav::getHeader()
{
    return &header;
}

template<typename T>
void SoundWav::readData(T* data, int nbItem) {
    if(_wav_file == nullptr)
        fclose(_wav_file);
    _wav_file = fopen(filename, "rb");
    if (!_wav_file) {
        char str[100];
        sprintf(str, "Unable to load file %s\n", filename);
        std::cout<<str<<std::endl;
        return;
    }
    fseek(_wav_file, sizeHeader, SEEK_SET);
    if(header.bits_per_sample == 16)
    {
        short dataShort[nbItem];
        fread(dataShort, sizeof(short), nbItem, _wav_file);
        for (int ID_t = 0; ID_t<nbItem; ++ID_t)
        {
            *(data+ID_t) = (float) dataShort[ID_t] / (float)(INT16_MAX + 1);
        }
    }
    else if(header.bits_per_sample == 8)
    {
        char dataChar[nbItem];
        fread(dataChar, sizeof(char), nbItem, _wav_file);
        for (int ID_t = 0; ID_t<nbItem; ++ID_t)
            *(data+ID_t)  =  dataChar[ID_t] / (float)(INT8_MAX + 1);
    }
    fclose(_wav_file);
}


int SoundWav::createWavFile(char *databasePath)
{
    filename = databasePath;
    sizeData = 0;

    header.riff_id = ID_RIFF;
    header.riff_sz = 0;

    header.wave_id = ID_WAVE;
    header.fmt_id = ID_FMT;
    header.fmt_sz = 16;

    header.audio_format = FORMAT_FLOAT;//FORMAT_PCM;
    header.num_channels = 2;
    header.sample_rate = 44100;
    header.bits_per_sample = 32;//16;
    header.byte_rate = (header.bits_per_sample / 8) * header.num_channels * header.sample_rate;
    header.block_align = header.num_channels * (header.bits_per_sample / 8);


    header.data_id = ID_DATA;
    if(_wav_file != nullptr)
    {
        fclose(_wav_file);
    }
    _wav_file = fopen(databasePath, "wb");
    if (!_wav_file) {
        char str[100];
        sprintf(str, "Unable to create file %s\n", databasePath);
        printf(str);
        return 1;
    }

    fseek(_wav_file, sizeof(struct Wav_header), SEEK_SET);
}

void WavHelpers::closeWavFile() {
    if(_wav_file == nullptr)
    {
        printf("The file is already closed");
    }
    header.data_sz = sizeData * header.block_align;
    header.riff_sz = header.data_sz + sizeof(header) - 8;
    fseek(_wav_file, 0, SEEK_SET);
    fwrite(&header, sizeof(struct Wav_header), 1, _wav_file);
    fclose(_wav_file);
}

int WavHelpers::addByteToWave(char* data, unsigned int nbByte) {
    if(_wav_file == nullptr)
    {
        printf("The file is already closed");
    }
    if (fwrite(data, 1, nbByte, _wav_file) != nbByte) {
        printf("Error capturing sample\n");
        return 0;
    }
    sizeData += (unsigned int)(nbByte/8);
    return 1;
}
