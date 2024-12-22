#pragma once

#include <sound/system.hpp>

namespace Sounds {

	void load();

	extern SoundBuffer soft_1;
	extern SoundBuffer soft_2;
	extern SoundBuffer soft_3;
	extern SoundBuffer soft_4;
	extern SoundBuffer damage_1;
	extern SoundBuffer damage_2;
	extern SoundBuffer damage_3;
	extern SoundBuffer damage_4;
	extern SoundBuffer blow_1;
	extern SoundBuffer blow_2;
	extern SoundBuffer death;
	extern SoundBuffer coin;
	extern SoundBuffer beep;

	SoundBuffer& getRandomBlow();
	SoundBuffer& getRandomDamage();
	SoundBuffer& getRandomSoft();

};
