#pragma once

#include <external.hpp>

enum struct SoundGroup {
	MASTER,
	MUSIC,
	GAME,
	GUI
};

class SoundVolumes {

	private:

		std::unordered_map<SoundGroup, float> volumes;

	public:

		float get(SoundGroup group) {
			if (group == SoundGroup::MASTER) {
				return 1.0f;
			}

			return volumes[group];
		}

		void set(SoundGroup group, float value) {
			volumes[group] = value;
		}

};
