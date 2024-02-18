#pragma once

#include <external.hpp>
#include <sound/debug.hpp>
#include <sound/event.hpp>
#include <sound/volume.hpp>

class SoundSource {

	private:

		friend class SoundSystem;

		uint32_t source;
		const char* path;
		std::list<SoundEvent> events;
		SoundGroup group = SoundGroup::MASTER;
		SoundVolumes& volumes;
		float gain;

	public:

		SoundSource(const SoundBuffer& sound, SoundVolumes& volumes)
		: path(sound.identifier()), volumes(volumes) {
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

		const char* identifier() const {
			return path;
		}

		bool should_drop() const {
			int state;
			alGetSourcei(source, AL_SOURCE_STATE, &state);

			return state == AL_STOPPED;
		}

		void update() {
			float current = seconds();
			std::list<SoundEvent>::iterator iter = events.begin();

			while (iter != events.end()) {
				bool drop = (*iter).update(*this, current);

				if (drop) {
					iter = events.erase(iter);
				} else {
					iter ++;
				}
			}
		}

	// play state menegment
	public:

		void play() {
			alSourcef(source, AL_GAIN, gain * volumes.get(group));
			debug::openal::check_error("alSourcef");

			alSourcePlay(source);
			debug::openal::check_error("alSourcePlay");
		}

		void pause() {
			alSourcePause(source);
			debug::openal::check_error("alSourcePause");
		}

		void drop() {
			alSourceStop(source);
			debug::openal::check_error("alSourceStop");
		}

	// source properties
	public:

		SoundSource& in(SoundGroup group) {
			this->group = group;
			return *this;
		}

		SoundSource& loop(bool value = true) {
			alSourcei(source, AL_LOOPING, value);
			debug::openal::check_error("alSourcei");
			return *this;
		}

		SoundSource& volume(float value) {
			this->gain = value;
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

		SoundSource& event(float seconds, SoundEvent::Callback callback, float window = 0.04f, bool once = true) {
			events.emplace_back(seconds, window, callback, once);
			return *this;
		}

		SoundSource& event(float seconds, Runnable runnable) {
			return event(seconds, [&] (float, SoundSource&) { runnable(); });
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
