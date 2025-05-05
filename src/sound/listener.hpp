
class SoundListener {

	public:

		// the position of the virtual listener
		SoundListener& position(float x, float y) {
			alListener3f(AL_POSITION, x, y, 0);
			debug::openal::check_error("alListenerfv");
			return *this;
		}

		// the velocity of the virtual listener
		SoundListener& velocity(float x, float y) {
			alListener3f(AL_VELOCITY, x, y, 0);
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
