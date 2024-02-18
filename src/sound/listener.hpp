
class SoundListener {

	public:

		// the position of the virtual listener
		SoundListener& position(glm::vec3 value) {
			alListenerfv(AL_POSITION, glm::value_ptr(value));
			debug::openal::check_error("alListenerfv");
			return *this;
		}

		// the velocity of the virtual listener
		SoundListener& velocity(glm::vec3 value) {
			alListenerfv(AL_VELOCITY, glm::value_ptr(value));
			debug::openal::check_error("alListenerfv");
			return *this;
		}

		// the "master volume"
		SoundListener& gain(float value) {
			alListenerf(AL_GAIN, value);
			debug::openal::check_error("alListenerf");
			return *this;
		}

};
