#pragma once

#include <external.hpp>

class SoundSource;

class SoundEvent {

	public:

		using Callback = std::function<void(float, SoundSource&)>;

	private:

		const float seconds;
		const float window;
		const Callback callback;
		const bool once;

		bool playable;

	public:

		SoundEvent(float seconds, float window, const Callback& callback, bool once)
		: seconds(seconds), window(window), callback(callback), once(once), playable(true) {}

		bool update(SoundSource& source, float current) {
			if ((current >= seconds) && (current <= seconds + window)) {
				if (playable) {
					playable = false;
					callback(current, source);
					return once;
				}
			} else {
				playable = true;
			}

			return false;
		}

};
