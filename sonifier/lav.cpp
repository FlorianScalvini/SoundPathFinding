#include "lav.h"

void lav::start() {

	lavLog::LAVLOG("start lav\n");
    lavVocal::init();
	lavVideoProcessor::init();
	lavComputer::init();
	lavConstants::init();
	lavSoundDatabase::init();
	lavAudioStream::init();
	lavSonifier::init();
	lavAudioMixer::init();
    lavManager::init();
    //lavAudioMixer::record_output("./res/hrtf_cut020_180H_90V/audio/trajectory/09_09_far.wav", 5000);
    lavLog::LAVLOG("lav init finished\n");
    //lavVideoProcessor::start_video_stream(NULL);

    usleep(3000000);

    lavLog::LAVLOG("lavVideoProcessor::start_thread_video_stream\n");
    lavVideoProcessor::start_thread_video_stream();

    lavLog::LAVLOG("lavAudioStream::start_thread_audio_stream\n");
    lavAudioStream::start_thread_audio_stream();

    lavLog::LAVLOG("lavAudioStream::start_thread_audio_stream\n");
    lavManager::start_thread_path_manager();

    lavManager::setDst(3);
}

void lav::stop() {
    lavLog::LAVLOG("lav::stop() !!!!!!!!!!\n");
    lavManager::release();
	lavAudioStream::release();
	lavVideoProcessor::release();
}

void lav::setDatabasePath(char* databasePath) {
    char str[200];
    sprintf(str, "setDatabasePath %s\n", databasePath);
	lavLog::LAVLOG(str);
	lavSoundDatabase::setDatabasePath(databasePath);
}

void lav::setDefaultDatabasePath(char* databasePath) {
    char str[200];
    sprintf(str, "setDefaultDatabasePath %s\n", databasePath);
	lavLog::LAVLOG(str);
	lavSoundDatabase::setDefaultDatabasePath(databasePath);
}

void lav::startOrStopSound() {
	lavLog::LAVLOG("startOrStopSound \n");
	lavVideoProcessor::startOrStopSound();
}

