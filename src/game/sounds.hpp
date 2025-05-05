#pragma once

#include "sound/system.hpp"

namespace Sounds {

	/// Sounds are loaded by this method, call it before trying to play any sound
	void load();

	inline SoundGroup beep;   // Death screen beep
	inline SoundGroup blow;   // Large explosion
	inline SoundGroup coin;   // Bonus pickup
	inline SoundGroup damage; // Damage (to player or enemy)
	inline SoundGroup death;  // Game over
	inline SoundGroup empty;  // Out of ammo trigger
	inline SoundGroup soft;   // Player shoot

};
