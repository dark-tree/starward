#pragma once
#include "level/level.hpp"

class Game {

	private:

		bool loaded = false;

		void load();
		void unload();
		void loadBiomes();

	public:

		BiomeManager* biomes = nullptr;
		Level* level = nullptr;

		Game();
		void tick();

};
