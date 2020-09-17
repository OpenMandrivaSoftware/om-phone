#pragma once

class AudioManager {
public:
	static AudioManager *get();
	void enableSpeaker();
	void enableEarpiece();
	void enableRouting();
	void enableMicrophone();

	void muteMicrophone();
	void muteSpeaker();
protected:
	AudioManager();
};
