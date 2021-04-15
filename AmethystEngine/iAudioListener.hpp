#pragma once
#include <fmod\fmod.hpp>

class iAudioListener {
public:
	virtual FMOD_VECTOR AudLisGetPosition() = 0;
	virtual FMOD_VECTOR AudLisGetVelocity() = 0;
	virtual FMOD_VECTOR AudLisGetForward() = 0;
	virtual FMOD_VECTOR AudLisGetUp() = 0;
};