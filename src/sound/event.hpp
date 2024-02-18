#pragma once

#include <external.hpp>

class SoundSource;

class SoundEvent {

	public:

		using Callback = std::function<void(float, SoundSource&)>;

	private:

		const float seconds;
		const Callback callback;

	public:

		SoundEvent(float seconds, const Callback& callback)
		: seconds(seconds), callback(callback) {}

		bool update(SoundSource& source, float current) {
			if (current >= seconds) {
				callback(current, source);
				return true;
			}

			return false;
		}

};
