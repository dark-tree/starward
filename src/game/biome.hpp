#pragma once

#include "external.hpp"
#include "util.hpp"

enum struct Alien {
	SWEEPER = 0,
	TURRET = 1,
	FIGHTER = 2,
	VERTICAL = 3,
};

enum struct Evolution {
	LOW = 0,
	MEDIUM = 1,
	HIGH = 2,
	HELLISH = 3,
};

struct EnemyPlacer {
	int count;
	int rarity;

	int pick() const;
};

struct Biome {

	public:

		float terrain_low = 0.0f;
		float terrain_high = 0.25;
		float scroll_speed = 0.0f;
		int bonus_rarity = 40;
		int end_segment = -1;

		std::vector<Evolution> evolutions;
		std::vector<Alien> aliens;
		std::vector<EnemyPlacer> placers;

	public:

		Biome();

		Evolution pickEvolution() const;
		Alien pickAlien() const;
		int pickCount() const;

};

class BiomeBuilder {

	private:

		Biome& biome;

	public:

		BiomeBuilder(Biome& biome);

		BiomeBuilder setPowerUpRarity(int rarity);
		BiomeBuilder setScrollSpeed(float speed);
		BiomeBuilder addEvolution(Evolution evolution, int weight);
		BiomeBuilder addAlien(Alien alien, int weight);
		BiomeBuilder addEnemyPlacer(int count, int rarity);
		BiomeBuilder setEndSegment(int segment);
		BiomeBuilder setTerrain(float low, float high);

};

class BiomeManager {

	private:

		float speed = 0;
		float low = -1, high = -1;

		int index = 0;
		std::vector<Biome> biomes;

	public:

		void reset();

		const Biome& current();
		void tick(int segment);
		BiomeBuilder beginBiome();

		Alien getAlien();
		Evolution getEvolution();
		int getEnemyCount();
		float getBonusSpeed();
		int getPowerUpRarity();
		glm::vec2 getTerrain();
		int getBiomeIndex();

};

