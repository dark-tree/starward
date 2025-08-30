
#include "level.hpp"

#include "game/sounds.hpp"
#include "game/emitter.hpp"
#include "biome.hpp"
#include "title.hpp"
#include "game/entity/all.hpp"
#include "game/entity/enemy/decay.hpp"

glm::vec2 Level::toTilePos(int x, int y) {
	const float pixels = SW / Segment::width;
	return glm::vec2 {x, y} / pixels;
}

glm::vec2 Level::toEntityPos(int x, int y) {
	const float pixels = SW / Segment::width;
	return (glm::vec2 {x, y} + 0.5f) * pixels;
}

Level::Level(BiomeManager& manager)
: manager(manager) {
	loadHighScore();
	manager.tick(0);
}

void Level::applyCustomSpawnLogic(Segment& segment) {

	int number = segment.index;
	int selector = number % 50;

	if (number < 20) {
		return;
	}

	if ((selector == 0) || (selector == 1)) {
		glm::ivec2 tile = segment.getRandomSpawnPos(0);
		glm::vec2 pos = toEntityPos(tile.x, tile.y);
		auto fighter = std::make_shared<FighterAlienEntity>(pos.x, pos.y, (int) manager.getEvolution());
		trySpawn(fighter);
	}

}

void Level::beginPlay() {
	if (!playing) {
		SoundSystem::getInstance().add(Sounds::music).volume(0.3).loop(true).play();
	}

	playing = true;
}

void Level::loadHighScore() {
	const char* stat_hi_score = "hi";
	std::string hi_str = platform_read_string(stat_hi_score);

	if (hi_str.length() > 0) {
		try {
			hi = std::stoi(hi_str);
		} catch (...) {
			hi = 0;
			platform_write_string(stat_hi_score, "0");
		}
	}
}

void Level::spawnInitial() {
	addEntity(new PlayerEntity {});

	float offset = toEntityPos(0, Segment::height * 3).y;
	addEntity(new SweeperAlienEntity {100, 450 + offset, 0});
	addEntity(new PowerUpEntity {200, 600 + offset, PowerUpEntity::LIVE});

	{
		int code = platform_get_startup_param();
		std::string line1 = getTitle(code);
		std::string line2 = getSubTitle(code);

		printf("Rendering title screen...\n");
		printf(" * Title:    %s\n", line1.c_str());
		printf(" * Subtitle: %s\n", line2.c_str());

		segments[1].fill(0);
		segments[1].generateString((Segment::width - (int) line2.size() * 8) / 2, (Segment::height / 2 - 8) / 2, line2, 4, 1);
		segments[1].generateString((Segment::width - (int) line1.size() * 8) / 2, Segment::height / 2 + (Segment::height / 2 - 8) / 2, line1, 4, 1);
	}

	// float oy = 1024;
	// int layers = 6;
	//
	// auto shared = std::make_shared<DecaySharedState>();
	//
	// for (int j = 0; j < 34; j ++) {
	// 	float k = 32.0f * j;
	//
	// 	int s = randomInt(-1, 1);
	// 	int e = randomInt(-1, 2);
	//
	// 	for (int i = s; i < layers + e; i ++) {
	// 		float o = 32.0f * i;
	//
	// 		addEntity(shared->createPart(k, oy + o));
	// 	}
	// }
	//
	// for (int i = randomInt(10, 15); i > 0; i --) {
	// 	float x = 32 * randomInt(0, 34);
	// 	float y = 32 * randomInt(9, 12);
	//
	// 	addEntity(shared->createPart(x, oy + y));
	// }
}

void Level::loadPlayCount() {

	if (play_count > 0) {
		printf("Looks like play count was already loaded, did the player respawn?\n");
		return;
	}

	constexpr const char* local_play_count = "count";

	int plays = 0;
	std::string plays_str = platform_read_string(local_play_count);

	if (plays_str.length() > 0) {
		plays = std::stoi(plays_str);
	}

	plays ++;
	platform_write_string(local_play_count, std::to_string(plays));

	printf("Started session #%d\n", plays);
	this->play_count = plays;

}

void Level::addSlowness(float tar) {
	this->tar *= tar;
}

void Level::addScore(int points) {
	if (state != GameState::DEAD) {
		score += points;

		if (points < 0) {
			score_lost_ticks = 30;
		}

		if (score < 0) {
			score = 0;
		}
	}
}

void Level::setState(GameState state) {
	this->state = state;

	if (state == GameState::DEAD) {
		if (score > hi) {
			printf("New hi-score set: %d points (was: %d points)!\n", score, hi);
			platform_write_string("hi", std::to_string(score));
		}
	}
}

bool Level::trySpawnAlien(Segment& segment) {
	Alien alien = manager.getAlien();
	int evolution = (int) manager.getEvolution();

	// basic aliens
	if (alien == Alien::SWEEPER) return AlienEntity::spawn<SweeperAlienEntity>(*this, segment, evolution);
	if (alien == Alien::VERTICAL) return AlienEntity::spawn<VerticalAlienEntity>(*this, segment, evolution);
	if (alien == Alien::MINE) return AlienEntity::spawn<MineAlienEntity>(*this, segment, evolution);
	if (alien == Alien::FIGHTER) return AlienEntity::spawn<FighterAlienEntity>(*this, segment, evolution);

	// aliens with custom spawners
	if (alien == Alien::TURRET) return TurretAlienEntity::spawn(*this, segment, evolution);
	if (alien == Alien::TESLA) return TeslaAlienEntity::spawn(*this, segment, evolution);

	printf("Failed to spawn due to invalid enum value, is the enemy spawn logic unimplemented?\n");
	return false;
}

std::shared_ptr<PlayerEntity> Level::getPlayer() {
	return player;
}

void Level::tick() {

	if (score_lost_ticks > 0) {
		score_lost_ticks --;
	}

	// update player pointer
	if (player && player->shouldRemove()) {
		player.reset();
	}

	if (state == GameState::DEAD) {
		age ++;

		aliveness = aliveness * 0.99f;
		linear_aliveness -= 0.005;

		if (linear_aliveness < 0) {
			linear_aliveness = 0;
		}
	} else {
		age = 60;
		aliveness = 1.0f;
		linear_aliveness = 1.0f;
	}

	biome_speed = manager.getBonusSpeed();
	scroll -= getSpeed();
	skip *= 0.95;
	tar = 1.0f;

	for (auto& segment : segments) {

		// returns true when it is regenerated, populate with entities
		if (segment.tick(scroll, manager.getTerrain())) {
			int count = manager.getEnemyCount();

			int attempts = 100;

			// try adding enemies
			for (int i = 0; i < count;) {
				if (trySpawnAlien(segment)) {
					i ++;
				}

				if (attempts -- < 0) {
					printf("Unable to fill spawn quota of %d for segment %d, aborting!\n", count, segment.index);
					break;
				}
			}

			// place powerups
			if (randomInt(0, manager.getPowerUpRarity()) == 0) {
				glm::ivec2 tile = segment.getRandomSpawnPos(6);
				glm::vec2 entity = toEntityPos(tile.x, tile.y);

				addEntity(new PowerUpEntity(entity.x, entity.y, PowerUpEntity::randomPick()));
			}

			applyCustomSpawnLogic(segment);

			// prepare for the next segment
			if (state != GameState::DEAD) {
				manager.tick(++total);
			}
		}
	}

	for (auto& entity : entities) {
		entity->tick(*this);
	}

	if (skip > 1) {
		skip = 1;
	}

	entities.erase(std::remove_if(entities.begin(), entities.end(), [] (const auto& entity) {
		return entity->shouldRemove();
	}), entities.end());

	for (auto& entity : pending) {
		entities.emplace_back(entity);
		entity->onSpawned(*this, findSegment(toTilePos(entity->x, entity->y).y));

		if (std::shared_ptr<PlayerEntity> shared_player = std::dynamic_pointer_cast<PlayerEntity>(entity)) {
			player = shared_player;
		}
	}

	pending.clear();

	if (Input::matchKeys(Key::UP, Key::UP, Key::DOWN, Key::DOWN, Key::LEFT, Key::RIGHT, Key::LEFT, Key::RIGHT, Key::B, Key::A)) {
		Input::purge();
		debug = !debug;
	}

	if (state == GameState::DEAD) {
		if (Input::isPressed(Key::ENTER)) {
			reload = true;
		}
	}
}

void Level::drawCredits(Renderer& renderer) {
	int half = SW/2;
	int row = SH/2 - SH/6;

	emitTextQuads(renderer.text, SW / 2, SH - 64, 32, 24, 255, 255, 0, 255, "Credits", TextMode::CENTER);

	// top left
	emitTextQuads(renderer.text, half / 2, SH - 64*2, 24, 20, 255, 255, 0, 255, "Sounds Effects", TextMode::CENTER);
	emitTextQuads(renderer.text, half / 2, SH - 64*2 - 32*2, 24, 20, 255, 255, 255, 255, "EVRetro", TextMode::CENTER);
	emitTextQuads(renderer.text, half / 2, SH - 64*2 - 32*3, 24, 20, 255, 255, 255, 255, "Sophia Caldwell", TextMode::CENTER);
	emitTextQuads(renderer.text, half / 2, SH - 64*2 - 32*4, 24, 20, 255, 255, 255, 255, "Cabled Mess", TextMode::CENTER);
	emitTextQuads(renderer.text, half / 2, SH - 64*2 - 32*5, 24, 20, 255, 255, 255, 255, "magistermaks", TextMode::CENTER);

	// top right
	emitTextQuads(renderer.text, half / 2 + half, SH - 64*2, 24, 20, 255, 255, 0, 255, "Programming & Art", TextMode::CENTER);
	emitTextQuads(renderer.text, half / 2 + half, SH - 64*2 - 32*2, 24, 20, 255, 255, 255, 255, "magistermaks", TextMode::CENTER);

	// bottom left
	emitTextQuads(renderer.text, half / 2, SH - 64*2 - row, 24, 20, 255, 255, 0, 255, "Shading", TextMode::CENTER);
	emitTextQuads(renderer.text, half / 2, SH - 64*2 - row - 32*2, 24, 20, 255, 255, 255, 255, "Mattias", TextMode::CENTER);

	// bottom right
	emitTextQuads(renderer.text, half / 2 + half, SH - 64*2 - row, 24, 20, 255, 255, 0, 220, "Music", TextMode::CENTER);
	emitTextQuads(renderer.text,  half / 2 + half, SH - 64*2 - row - 32*2, 24, 20, 255, 255, 255, 220, "Skay", TextMode::CENTER);

}


void Level::draw(Renderer& renderer) {

	for (auto& segment : segments) {
		segment.draw(renderer.terrain, scroll, debug);
	}

	for (auto& entity : entities) {
		entity->draw(*this, renderer);
	}

	if (debug) {
		for (auto& entity : entities) {
			entity->debugDraw(*this, renderer);
		}

		emitTextQuads(renderer.text, 16, SH - 64,  20, 16, 255, 255, 0, 220, "Seg: " + std::to_string(total), TextMode::LEFT);
		emitTextQuads(renderer.text, 16, SH - 96,  20, 16, 255, 255, 0, 220, "Bio: " + std::to_string(manager.getBiomeIndex()), TextMode::LEFT);
		emitTextQuads(renderer.text, 16, SH - 128, 20, 16, 255, 255, 0, 220, "Spd: " + std::to_string(getSpeed()), TextMode::LEFT);
		emitTextQuads(renderer.text, 16, SH - 160, 20, 16, 255, 255, 0, 220, "Ens: " + std::to_string(entities.size()), TextMode::LEFT);
	}

	if (state != GameState::DEAD) {
		uint8_t g = 255;
		uint8_t b = 0;

		if (score_lost_ticks > 0) {
			g = 0;

			if ((score_lost_ticks / 5) % 2 == 0) {
				g = 150;
				b = 150;
			}

		}

		emitTextQuads(renderer.text, SW - 16, SH - 32, 24, 20, 255, g, b, 220, std::to_string(score), TextMode::RIGHT);
	}

	if (state == GameState::DEAD && (age % 120 < 60)) {
		if (age % 120 == 0) {
			SoundSystem::getInstance().add(Sounds::beep).volume(1.5).play();
		}

		std::string over = "GAME OVER";

		int spacing = 8;
		int width = 48 + spacing;
		int start = (SW - (over.length() - 1) * width) / 2;

		emitTextQuads(renderer.text, start, SH / 2 + 24, width, 48, 255, 255, 0, 220, over, TextMode::LEFT);
		emitTextQuads(renderer.text, start - 8, SH / 2 + 16 - 48, 24, 20, 255, 255, 0, 220, "SCORE: " + std::to_string(score), TextMode::LEFT);

		if ((score > hi) && (age % 10 < 5)) {
			emitTextQuads(renderer.text, start - 8, SH / 2 + 16 - 48 - 32, 24, 20, 255, 255, 0, 220, "NEW HI-SCORE!", TextMode::LEFT);
		}
	}

	if (Input::isPressed(PlatformKeyScope::TAB)) {
		drawCredits(renderer);
	}

}

uint8_t Level::getTile(int tx, int ty) const {
	if (tx < 0 || tx >= Segment::width) {
		return 0;
	}

	if (const Segment* segment = findSegment(ty)) {
		return segment->atWorldPos(tx, ty);
	}

	return 0;
}

Segment* Level::findSegment(int y) {
	for (auto& segment : segments) {
		if (segment.contains(y)) {
			return &segment;
		}
	}

	return nullptr;
}

const Segment* Level::findSegment(int y) const {
	for (auto& segment : segments) {
		if (segment.contains(y)) {
			return &segment;
		}
	}

	return nullptr;
}

void Level::setTile(int tx, int ty, uint8_t tile) {
	if (tx < 0 || tx >= Segment::width) {
		return;
	}

	if (Segment* segment = findSegment(ty)) {
		segment->atWorldPos(tx, ty) = tile;
	}
}

float Level::getSkip() const {
	return skip;
}

float Level::getScroll() const {
	return scroll;
}

float Level::getSpeed() const {
	if (!playing) return 0.0f;
	float v = std::min(1.0f, total * 0.002f);
	return tar * base_speed * aliveness + (skip * 4 + tar * v * 2 + biome_speed) * aliveness;
}

float Level::getLinearAliveness() const {
	return linear_aliveness;
}

bool Level::isDebug() const {
	return debug;
}

Collision Level::checkTileCollision(const Box& box) const {

	// check if the collider is outside level bounds
	// we only check it horizontally as some terrain can be off-screen vertically
	if (box.x + box.w < 0 || box.x > SW) {
		return {};
	}

	// convert position to tile space
	glm::vec2 xyt = toTilePos(box.x, box.y);
	glm::vec2 wht = ceil(toTilePos(box.w, box.h));

	// convert to integers after addition for better precision
	glm::ivec2 pos = floor(xyt);
	glm::ivec2 end = ceil(xyt + wht);

	// check tiles in box
	for (int x = pos.x; x < end.x; x++) {
		for (int y = pos.y; y < end.y; y++) {
			uint8_t tile = getTile(x, y);

			if (tile) {
				return {x, y};
			}
		}
	}

	// no collision found
	return {};

}

Collision Level::checkEntityCollision(Entity* self) const {

	// handle alredy existing entities
	for (const auto& entity : entities) {
		Entity* pointer = entity.get();

		// skip invalid collisions
		if (pointer == self || pointer == nullptr) {
			continue;
		}

		// entity collisions are handled by entities
		if (pointer->shouldCollide(self)) {
			return {pointer};
		}
	}

	// handle entities added in this frame
	for (const auto& entity : pending) {
		Entity* pointer = entity.get();

		if (pointer == self || pointer == nullptr) {
			continue;
		}

		if (pointer->shouldCollide(self)) {
			return {pointer};
		}
	}

	// no collision found
	return {};

}

Collision Level::checkCollision(Entity* self) const {

	// check for terrain collision first as it is faster
	Collision collision = checkTileCollision(self->getBoxCollider());

	// if terrain check fail, try checking for entity collision
	if (collision.type == Collision::MISS) {
		collision = checkEntityCollision(self);
	}

	return collision;

}

std::vector<std::shared_ptr<Entity>>& Level::getEntities() {
	return entities;
}