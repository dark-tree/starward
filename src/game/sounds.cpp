
#include "sounds.hpp"
#include "util.hpp"

void Sounds::load() {
	soft_1 = SoundBuffer {"assets/sound/soft-1.ogg"};
	soft_2 = SoundBuffer {"assets/sound/soft-2.ogg"};
	soft_3 = SoundBuffer {"assets/sound/soft-3.ogg"};
	soft_4 = SoundBuffer {"assets/sound/soft-4.ogg"};
	damage_1 = SoundBuffer {"assets/sound/damage-1.ogg"};
	damage_2 = SoundBuffer {"assets/sound/damage-2.ogg"};
	damage_3 = SoundBuffer {"assets/sound/damage-3.ogg"};
	damage_4 = SoundBuffer {"assets/sound/damage-4.ogg"};
	blow_1 = SoundBuffer {"assets/sound/blow-1.ogg"};
	blow_2 = SoundBuffer {"assets/sound/blow-2.ogg"};
	death = SoundBuffer {"assets/sound/death.ogg"};
	coin = SoundBuffer {"assets/sound/coin.ogg"};
}

SoundBuffer& Sounds::getRandomBlow() {
	SoundBuffer* buffers[] = {&blow_1, &blow_2};
	return *buffers[randomInt(0, 1)];
}

SoundBuffer& Sounds::getRandomDamage() {
	SoundBuffer* buffers[] = {&damage_1, &damage_2, &damage_3, &damage_4};
	return *buffers[randomInt(0, 3)];
}

SoundBuffer& Sounds::getRandomSoft() {
	SoundBuffer* buffers[] = {&soft_1, &soft_2, &soft_3, &soft_4};
	return *buffers[randomInt(0, 3)];
}

SoundBuffer Sounds::soft_1 {};
SoundBuffer Sounds::soft_2 {};
SoundBuffer Sounds::soft_3 {};
SoundBuffer Sounds::soft_4 {};
SoundBuffer Sounds::damage_1 {};
SoundBuffer Sounds::damage_2 {};
SoundBuffer Sounds::damage_3 {};
SoundBuffer Sounds::damage_4 {};
SoundBuffer Sounds::blow_1 {};
SoundBuffer Sounds::blow_2 {};
SoundBuffer Sounds::death {};
SoundBuffer Sounds::coin {};
