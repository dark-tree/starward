#pragma once

#include <sound/system.hpp>

namespace Sounds {

	/// Sounds are loaded by this method, call it before trying to play any sound
	void load();

	extern SoundGroup beep;   // Death screen beep
	extern SoundGroup blow;   // Large explosion
	extern SoundGroup coin;   // Bonus pickup
	extern SoundGroup damage; // Damage (to player or enemy)
	extern SoundGroup death;  // Game over
	extern SoundGroup empty;  // Out of ammo trigger
	extern SoundGroup soft;   // Player shoot

};
