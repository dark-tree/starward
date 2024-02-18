#pragma once

#include <external.hpp>
#include <sound/debug.hpp>

class SoundBuffer {

	private:

		friend class SoundSource;

		uint32_t buffer;
		const char* path;

		uint32_t format_of(uint32_t channels) {
			return (channels > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
		}

	public:

		SoundBuffer(const char* path)
		: path(path) {
			alGenBuffers(1, &buffer);
			debug::openal::check_error("alGenBuffers");

			int format = 0;
			int channels;
			int samples;
			short* data;

			uint32_t count = stb_vorbis_decode_filename(path, &channels, &samples, &data);

			printf("Uploading sound: '%s', count: %d, channels: %d, freq: %d\n", path, count, channels, samples);

			if (count == -1) {
				fault("Failed to load sound: '%s'\n", path);
			}

			uint32_t length = count * channels * sizeof(short);
			alBufferData(buffer, format_of(channels), data, length, samples);
			debug::openal::check_error("alBufferData");

			if (format == AL_FORMAT_STEREO16) {
				printf("Attenuation not supported for stereo sound: '%s'!\n", path);
			}

			// the documentation for STB vorbis is non-existant
			// i assume this is what should be done, other STB libraries typically had a
			// custom free function but for this one i couldn't find any
			free(data);
		}

		~SoundBuffer() {
			alDeleteBuffers(1, &buffer);
			debug::openal::check_error("alDeleteBuffers");
		}

		const char* identifier() const {
			return path;
		}
};
