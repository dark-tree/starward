#pragma once

#include <external.hpp>

enum struct SoundChannel {
	MASTER,
	MUSIC,
	GAME,
	GUI
};

class SoundVolumes {

	private:

		std::unordered_map<SoundChannel, float> volumes;

	public:

		float get(SoundChannel group) {
			if (group == SoundChannel::MASTER) {
				return 1.0f;
			}

			return volumes[group];
		}

		void set(SoundChannel group, float value) {
			volumes[group] = value;
		}

};
