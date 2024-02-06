#pragma once

#include <external.hpp>
#include "system.hpp"

class SoundBuffer {

	private:

		friend class SoundSource;

		uint32_t buffer;

		uint32_t format_of(uint32_t channels) {
			return (channels > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
		}


	public:

		SoundBuffer() {
			alGenBuffers(1, &buffer);
			sound::check_error("alGenBuffers");
		}

		~SoundBuffer() {
			alDeleteBuffers(1, &buffer);
		}

		void load(const char* path) {
			int format = 0;
			int channels;
			int samples;
			short* data;

			uint32_t count = stb_vorbis_decode_filename(path, &channels, &samples, &data);

			printf("Loaded sound: '%s', count: %d, channels: %d, freq: %d\n", path, count, channels, samples);

			if (count == -1) {
				fault("Failed to load sound: '%s'\n", path);
			}

			uint32_t length = count * channels * sizeof(short);
			alBufferData(buffer, format_of(channels), data, length, samples);
			sound::check_error("alBufferData");

			if (format == AL_FORMAT_STEREO16) {
				printf("Attenuation not supported for stereo sound: '%s'!\n", path);
			}
		}
};
