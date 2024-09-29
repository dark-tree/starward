
#include "sounds.hpp"

void Sounds::load() {
	hit_0 = SoundBuffer {"assets/sound/hit-0.ogg"};
	hit_1 = SoundBuffer {"assets/sound/hit-1.ogg"};
	hit_2 = SoundBuffer {"assets/sound/hit-2.ogg"};
	hit_3 = SoundBuffer {"assets/sound/hit-3.ogg"};
	hit_4 = SoundBuffer {"assets/sound/hit-4.ogg"};
	hit_5 = SoundBuffer {"assets/sound/hit-5.ogg"};
	hit_6 = SoundBuffer {"assets/sound/hit-6.ogg"};
	hit_7 = SoundBuffer {"assets/sound/hit-7.ogg"};
	hit_8 = SoundBuffer {"assets/sound/hit-8.ogg"};
	hit_9 = SoundBuffer {"assets/sound/hit-9.ogg"};
}

SoundBuffer Sounds::hit_0 {};
SoundBuffer Sounds::hit_1 {};
SoundBuffer Sounds::hit_2 {};
SoundBuffer Sounds::hit_3 {};
SoundBuffer Sounds::hit_4 {};
SoundBuffer Sounds::hit_5 {};
SoundBuffer Sounds::hit_6 {};
SoundBuffer Sounds::hit_7 {};
SoundBuffer Sounds::hit_8 {};
SoundBuffer Sounds::hit_9 {};

