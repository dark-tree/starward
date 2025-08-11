#pragma once

#include <external.hpp>
#include <sound/debug.hpp>
#include <sound/event.hpp>
#include <sound/buffer.hpp>

class SoundSource {

	private:

		friend class SoundSystem;

		int lastSample = 0;
		bool firstCleanupAttempt = true;

		uint32_t source;
		const std::string path;

	public:

		SoundSource(const SoundBuffer& sound)
		: path(sound.identifier()) {
			alGenSources(1, &source);
			debug::openal::check_error("alGenSources");

			alSourcef(source, AL_REFERENCE_DISTANCE, 5);
			alSourcei(source, AL_BUFFER, sound.buffer);
			alSource3f(source, AL_POSITION, 0, 0, 0);
			debug::openal::check_error("alSourcef");

			volume(1.0f);
		}

		~SoundSource() {
			alDeleteSources(1, &source);
			debug::openal::check_error("alDeleteSources");
		}

		const std::string& identifier() const {
			return path;
		}

		bool shouldDrop() {
			int state;
			alGetSourcei(source, AL_SOURCE_STATE, &state);

			// this is to avoid hundreds of sounds being
			// played at once if the user doesn't immediately click the website
			if (!firstCleanupAttempt && samples() == 0 && state == AL_PLAYING) {
				return true;
			}

			firstCleanupAttempt = false;
			return state == AL_STOPPED;
		}

	// play state management
	public:

		SoundSource& play() {
			alSourcePlay(source);
			debug::openal::check_error("alSourcePlay");
			return *this;
		}

		SoundSource& pause() {
			alSourcePause(source);
			debug::openal::check_error("alSourcePause");
			return *this;
		}

		SoundSource& drop() {
			alSourceStop(source);
			debug::openal::check_error("alSourceStop");
			return *this;
		}

	// source properties
	public:

		SoundSource& loop(bool value = true) {
			alSourcei(source, AL_LOOPING, value);
			debug::openal::check_error("alSourcei");
			return *this;
		}

		SoundSource& volume(float value) {
			alSourcef(source, AL_GAIN, value);
			debug::openal::check_error("alSourcef");
			return *this;
		}

		SoundSource& pitch(float value) {
			alSourcef(source, AL_PITCH, value);
			debug::openal::check_error("alSourcef");
			return *this;
		}

		SoundSource& position(glm::vec3 value) {
			alSourcefv(source, AL_POSITION, glm::value_ptr(value));
			debug::openal::check_error("alSourcefv");
			return *this;
		}

		SoundSource& velocity(glm::vec3 value) {
			alSourcefv(source, AL_VELOCITY, glm::value_ptr(value));
			debug::openal::check_error("alSourcefv");
			return *this;
		}

		SoundSource& direction(glm::vec3 value) {
			alSourcefv(source, AL_DIRECTION, glm::value_ptr(value));
			debug::openal::check_error("alSourcefv");
			return *this;
		}

	// state getters
	public:

		int samples() {
			int value;
			alGetSourcei(source, AL_SAMPLE_OFFSET, &value);
			debug::openal::check_error("alGetSourcei");

			return value;
		}

		float seconds() {
			float value;
			alGetSourcef(source, AL_SEC_OFFSET, &value);
			debug::openal::check_error("alGetSourcef");

			return value;
		}
};
