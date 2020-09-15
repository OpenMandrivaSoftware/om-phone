/**
 * Haptic feedback for devices supporting the
 * evdev force feedback feature
 * (C) 2020 Bernhard Rosenkränzer <bero@lindev.ch>
 * Released under the GPLv3
 */
#pragma once

#include <QString>

extern "C" {
#include <linux/input.h>
#include <unistd.h>
}

class HapticFeedback {
public:
	HapticFeedback(QString const &device=QString());
	~HapticFeedback() { if (_fd >= 0) close(_fd); }
	bool isOk() const { return _fd >= 0; }
public:
	void start();
	void stop();
private:
	bool sendCmd(bool start);
private:
	int _fd;
	ff_effect _effect[1];
};
