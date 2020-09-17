#include "AudioManager.h"
#include <QProcess>

AudioManager *AudioManager::get() {
	static AudioManager *instance=new AudioManager();
	return instance;
}

// FIXME anything below is PinePhone specific for now
void AudioManager::enableSpeaker() {
	QProcess::execute("/usr/bin/pinephone-audio-setup", QStringList() << "-s");
	QProcess::execute("/usr/bin/amixer", QStringList() << "set" << "Line Out" << "on");
}

void AudioManager::enableEarpiece() {
	QProcess::execute("/usr/bin/pinephone-audio-setup", QStringList() << "-e");
	QProcess::execute("/usr/bin/amixer", QStringList() << "set" << "Line Out" << "off");
}

void AudioManager::enableRouting() {
	QProcess::execute("/usr/bin/pinephone-audio-setup", QStringList() << "-2");
}

void AudioManager::enableMicrophone() {
	QProcess::execute("/usr/bin/pinephone-audio-setup", QStringList() << "-m");
	QProcess::execute("/usr/bin/amixer", QStringList() << "set" << "Mic1 Boost" << "2");
}

void AudioManager::muteMicrophone() {
	QProcess::execute("/usr/bin/amixer", QStringList() << "set" << "Mic1 Boost" << "0");
}

void AudioManager::muteSpeaker() {
	QProcess::execute("/usr/bin/amixer", QStringList() << "set" << "Line Out" << "off");
}

AudioManager::AudioManager() {
}
