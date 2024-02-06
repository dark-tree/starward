#pragma once

#include <external.hpp>
#include "buffer.hpp"

class SoundSource {

	private:

		friend class SoundBuffer;

		uint32_t source;

	public:

		SoundSource(const SoundBuffer& sound) {
			alGenSources(1, &source);

			alSourcef(source, AL_REFERENCE_DISTANCE, 5);
			alSourcei(source, AL_BUFFER, sound.buffer);
			alSource3f(source, AL_POSITION, 0, 0, 0);
			sound::check_error("alGenSources");
		}

		~SoundSource() {
			// TODO alDeleteSources(1, &source);
		}

		void play() {
			alSourcePlay(source);
			sound::check_error("alSourcePlay");
		}


};
