#pragma once

#include <external.hpp>
//#include "buffer.hpp"
//#include "source.hpp"

// to learn all the Audio api quirks
// see: https://emscripten.org/docs/porting/Audio.html

namespace sound {

	inline const char* get_error() {
		int error = alGetError();

		if (error == AL_NO_ERROR) {
			return nullptr;
		}

		if (error == AL_INVALID_NAME) return "AL_INVALID_NAME";
		if (error == AL_INVALID_ENUM) return "AL_INVALID_ENUM";
		if (error == AL_INVALID_VALUE) return "AL_INVALID_VALUE";
		if (error == AL_INVALID_OPERATION) return "AL_INVALID_OPERATION";
		if (error == AL_OUT_OF_MEMORY) return "AL_OUT_OF_MEMORY";

		return "UNDEFINED";
	}

	inline void check_error(const char* usage) {
		const char* error = get_error();

		if (error != nullptr) {
			fault("%s: OpenAL error '%s' occured!\n", usage, error);
		}
	}

	inline void init() {
		ALCdevice* device = alcOpenDevice(nullptr);

		if (device == nullptr) {
			fault("Sound system failed to start, unable to open device!\n");
		}

		ALCcontext* context = alcCreateContext(device, nullptr);

		if (context == nullptr) {
			fault("Sound system failed to start, unable to create context!\n");
		}

		alcMakeContextCurrent(context);
		printf("Sound system engaged!\n");
		check_error("init");
	}

};
