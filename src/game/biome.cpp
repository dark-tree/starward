#include "biome.hpp"

/*
 * EnemyPlacer
 */

int EnemyPlacer::pick() const {
	if ((rarity == 0) || (randomInt(0, rarity) == 0)) {
		return count;
	}

	return 0;
}

/*
 * Biome
 */

Biome::Biome() {

}

Evolution Biome::pickEvolution() const {
	return evolutions.at(randomInt(0, evolutions.size() - 1));
}

Alien Biome::pickAlien() const {
	return aliens.at(randomInt(0, aliens.size() - 1));
}

int Biome::pickCount() const {
	int count = 0;

	for (EnemyPlacer placer : placers) {
		count += placer.pick();
	}

	return count;
}

/*
 * BiomeBuilder
 */

BiomeBuilder::BiomeBuilder(Biome& biome)
: biome(biome) {}

BiomeBuilder BiomeBuilder::setPowerUpRarity(int rarity) {
	biome.bonus_rarity = rarity;
	return *this;
}

BiomeBuilder BiomeBuilder::setScrollSpeed(float speed) {
	biome.scroll_speed = speed;
	return *this;
}

BiomeBuilder BiomeBuilder::addEvolution(Evolution evolution, int weight) {
	for (int i = 0; i < weight; i ++) {
		biome.evolutions.push_back(evolution);
	}
	return *this;
}

BiomeBuilder BiomeBuilder::addAlien(Alien alien, int weight) {
	for (int i = 0; i < weight; i ++) {
		biome.aliens.push_back(alien);
	}
	return *this;
}

BiomeBuilder BiomeBuilder::addEnemyPlacer(int count, int rarity) {
	biome.placers.emplace_back(count, rarity);
	return *this;
}

BiomeBuilder BiomeBuilder::setEndSegment(int segment) {
	biome.end_segment = segment;
	return *this;
}

BiomeBuilder BiomeBuilder::setTerrain(float low, float high) {
	biome.terrain_low = low;
	biome.terrain_high = high;
	return *this;
}

/*
 * BiomeManager
 */

void BiomeManager::reset() {
	speed = 0;
	low = -1;
	high = -1;
	index = 0;
}

const Biome& BiomeManager::current() {
	return biomes[index];
}

void BiomeManager::tick(int segment) {

	const Biome& biome = current();
	float next_low = biome.terrain_low;
	float next_high = biome.terrain_high;

	if (low == -1 || high == -1) {
		low = next_low;
		high = next_high;
		return;
	}

	float factor = 0.73;
	this->low = std::lerp(next_low, low, factor);
	this->high = std::lerp(next_high, high, factor);

	if ((biome.end_segment != -1) && (segment >= biome.end_segment)) {
		index ++;
		printf("Switching biome to #%d, next switch at segment %d\n", index, current().end_segment);
	}

}

BiomeBuilder BiomeManager::beginBiome() {
	return {biomes.emplace_back()};
}

Alien BiomeManager::getAlien() {
	return current().pickAlien();
}

Evolution BiomeManager::getEvolution() {
	return current().pickEvolution();
}

int BiomeManager::getEnemyCount() {
	return current().pickCount();
}

float BiomeManager::getBonusSpeed() {
	speed = std::lerp(current().scroll_speed, speed, 0.99f);
	return speed;
}

int BiomeManager::getPowerUpRarity() {
	return current().bonus_rarity;
}

glm::vec2 BiomeManager::getTerrain() {
	return {low, high};
}

int BiomeManager::getBiomeIndex() {
	return index;
}
