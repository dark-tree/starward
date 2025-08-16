#include "game.hpp"

Game::Game() {
	load();
}

void Game::load() {

	// reset terrain generator
	Segment::resetTerrainGeneratr();

	biomes = new BiomeManager();
	loadBiomes();

	level = new Level(*biomes);
	level->spawnInitial();

	loaded = true;
}

void Game::unload() {
	delete level;
	delete biomes;
	loaded = false;
}

void Game::tick() {
	if (!loaded) {
		printf("Called tick() in unloaded context!\n");
		return;
	}

	if (level->reload) {
		unload();
		load();
		printf("Game context reloaded!\n");
		return;
	}

	level->tick();
}

void Game::loadBiomes() {

	biomes->beginBiome() // title
		.setBonusScrollSpeed(0)
		.setPowerUpRarity(1000)
		.setEndSegment(3);

	biomes->beginBiome() // start
		.setTerrain(0.0, 0.25)
		.addAlien(Alien::MINE, 1)
		.addEvolution(Evolution::LOW, 1)
		.addEvolution(Evolution::MEDIUM, 1)
		.addEvolution(Evolution::HIGH, 1)
		.addEnemyPlacer(1, 0)
		.setPowerUpRarity(10)
		.setBonusScrollSpeed(0.15)
		.setEndSegment(5);

	biomes->beginBiome() // sweepers
		.setTerrain(0.0, 0.25)
		.addAlien(Alien::SWEEPER, 2)
		.addAlien(Alien::MINE, 1)
		.addAlien(Alien::TESLA, 3)
		.addEvolution(Evolution::LOW, 1)
		.addEvolution(Evolution::MEDIUM, 1)
		.addEnemyPlacer(1, 0)
		.setPowerUpRarity(10)
		.setBonusScrollSpeed(0.1)
		.setEndSegment(20);

	biomes->beginBiome() // tight tesla twister
		.setTerrain(0.1, 0.4)
		.addAlien(Alien::TESLA, 2)
		.addAlien(Alien::SWEEPER, 1)
		.addEvolution(Evolution::LOW, 1)
		.setEndSegment(31)
		.addEnemyPlacer(1, 0);

	biomes->beginBiome() // turret introduced
		.setTerrain(0.0, 0.27)
		.addAlien(Alien::SWEEPER, 3)
		.addAlien(Alien::TURRET, 1)
		.addAlien(Alien::MINE, 1)
		.addAlien(Alien::TESLA, 3)
		.addEvolution(Evolution::LOW, 1)
		.addEvolution(Evolution::MEDIUM, 1)
		.addEnemyPlacer(1, 0)
		.addEnemyPlacer(1, 4)
		.setPowerUpRarity(10)
		.setEndSegment(60);

	biomes->beginBiome() // strong sweepers
		.setTerrain(0.1, 0.28)
		.addAlien(Alien::SWEEPER, 3)
		.addEvolution(Evolution::HIGH, 1)
		.addEnemyPlacer(1, 0)
		.addEnemyPlacer(1, 2)
		.setPowerUpRarity(6)
		.setEndSegment(72);

	biomes->beginBiome() // mixed bag
		.setTerrain(0.0, 0.26)
		.addAlien(Alien::SWEEPER, 3)
		.addAlien(Alien::TURRET, 2)
		.addAlien(Alien::MINE, 1)
		.addAlien(Alien::VERTICAL, 2)
		.addAlien(Alien::TESLA, 2)
		.addEvolution(Evolution::LOW, 1)
		.addEvolution(Evolution::MEDIUM, 3)
		.addEnemyPlacer(1, 0)
		.addEnemyPlacer(1, 4)
		.setPowerUpRarity(10)
		.setEndSegment(100);

	biomes->beginBiome() // now onto the HIGH evolution!
		.setTerrain(0.0, 0.26)
		.addAlien(Alien::SWEEPER, 1)
		.addAlien(Alien::TURRET, 1)
		.addEvolution(Evolution::LOW, 1)
		.addEvolution(Evolution::MEDIUM, 4)
		.addEvolution(Evolution::HIGH, 1)
		.addEnemyPlacer(1, 0)
		.addEnemyPlacer(1, 4)
		.setPowerUpRarity(10)
		.setEndSegment(130);

	biomes->beginBiome() // i heard you like turrets?
		.setTerrain(0.0, 0.27)
		.addAlien(Alien::SWEEPER, 1)
		.addAlien(Alien::TURRET, 5)
		.addEvolution(Evolution::LOW, 2)
		.addEvolution(Evolution::MEDIUM, 2)
		.addEvolution(Evolution::HIGH, 1)
		.addEnemyPlacer(1, 0)
		.addEnemyPlacer(1, 4)
		.setPowerUpRarity(9)
		.setEndSegment(140);

	biomes->beginBiome() // fun time
		.setTerrain(0.0, 0.26)
		.addAlien(Alien::SWEEPER, 3)
		.addAlien(Alien::TURRET, 3)
		.addAlien(Alien::VERTICAL, 2)
		.addAlien(Alien::MINE, 1)
		.addEvolution(Evolution::MEDIUM, 3)
		.addEvolution(Evolution::HIGH, 1)
		.addEnemyPlacer(1, 0)
		.addEnemyPlacer(1, 5)
		.setPowerUpRarity(10)
		.setEndSegment(-1);
}