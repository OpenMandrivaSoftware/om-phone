/* Class to drive the PinePhone (and similar) vibrator
 *
 * (C) 2020 Bernhard Rosenkränzer <bero@lindev.ch>
 *
 * Released under the GPLv3
 */

#include "HapticFeedback.h"
#include "BitField.h"

#include <QDir>
#include <QFile>
#include <iostream>

extern "C" {
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
}

HapticFeedback::HapticFeedback(QString const &device) {
	BitField<FF_MAX> ffFeatures;
	if(device.isEmpty()) {
		// Locate the vibrator -- it'll typically be the first
		// device that supports the force feedback API and doesn't
		// look like a joystick/gamepad.
		QDir devinput(QString::fromLatin1("/dev/input"));
		for(QString const &dev : devinput.entryList(QStringList() << QString::fromLatin1("event*"), QDir::Readable|QDir::Writable|QDir::System|QDir::NoDotAndDotDot)) {
			QString d(QString::fromLatin1("/dev/input/") + dev);
			_fd = open(QFile::encodeName(d).data(), O_RDWR);
			if(_fd < 0) {
				// If we can't open it, can't be the device
				continue;
			}

			int effects;
			if(ioctl(_fd, EVIOCGEFFECTS, &effects) < 0) {
				perror("EVIOCGEFFECTS");
				close(_fd);
				continue;
			}
			if(effects <= 0)
				continue;

			if(ioctl(_fd, EVIOCGBIT(EV_FF, sizeof(ffFeatures)), &ffFeatures) < 0) {
				perror("EV_FF");
				close(_fd);
				continue;
			}
			if(ffFeatures.isSet(FF_AUTOCENTER)) {
				// Looks like a joystick -- a vibrator
				// doesn't really have a center...
				close(_fd);
				continue;
			}
			if(!ffFeatures.isSet(FF_RUMBLE)) {
				// Doesn't support the mode we use (for now)
				// let's see if there's another device that does...
				close(_fd);
				continue;
			}
			break;
		}
	} else {
		_fd = open(QFile::encodeName(device).data(), O_RDWR);
		if(ioctl(_fd, EVIOCGBIT(EV_FF, sizeof(ffFeatures)), &ffFeatures) < 0) {
			perror("EV_FF");
			close(_fd);
			return;
		}
	}

	if(_fd < 0)
		return;

	if(ffFeatures.isSet(FF_GAIN)) {
		// set gain to 75%
		input_event gain;
		memset(&gain, 0, sizeof(gain));
		gain.type = EV_FF;
		gain.code = FF_GAIN;
		gain.value = 0xc000;
		if(write(_fd, &gain, sizeof(gain)) != sizeof(gain))
			perror("Set gain"); // Probably not fatal...
	}

	memset(&_effect, 0, sizeof(_effect));
	_effect[0].type = FF_RUMBLE;
	_effect[0].id = -1;
	_effect[0].u.rumble.strong_magnitude = 0xffff;
	_effect[0].u.rumble.weak_magnitude = 0x0;
	_effect[0].replay.length = 100;
	_effect[0].replay.delay = 0;
	if(ioctl(_fd, EVIOCSFF, &_effect[0]) < 0) {
		qDebug("EVIOCSFF: %s", strerror(errno));
	}

	sendCmd(true);
}

bool HapticFeedback::sendCmd(bool start) {
	input_event cmd;
	memset(&cmd, 0, sizeof(input_event));
	cmd.type = EV_FF;
	cmd.code = _effect[0].id;
	cmd.value = start ? 1 : 0;
	bool ok = write(_fd, &cmd, sizeof(cmd)) == sizeof(cmd);
	if(!ok) {
		qDebug("EV_FF: %s", strerror(errno));
	}
	return ok;
}

void HapticFeedback::start() {
	sendCmd(true);
}

void HapticFeedback::stop() {
	sendCmd(false);
}
