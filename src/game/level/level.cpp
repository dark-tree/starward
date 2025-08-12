
#include "level.hpp"

#include "game/sounds.hpp"
#include "game/emitter.hpp"
#include "biome.hpp"
#include "game/entity/all.hpp"
#include "game/entity/enemy/decay.hpp"

Level::Level(BiomeManager& manager)
: manager(manager) {
	loadHighScore();
	manager.tick(0);
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
	addEntity(new SweeperAlienEntity {100, 450, 0});
	addEntity(new PowerUpEntity {200, 600, PowerUpEntity::LIVE});

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
		this->score += points;
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

Entity* Level::randomAlien(int margin, LevelSegment& segment) {
	Alien alien = manager.getAlien();
	Evolution evolution = manager.getEvolution();

	if (alien == Alien::SWEEPER) {
		glm::ivec2 tile = segment.getRandomPos(margin);
		glm::vec2 pos = toEntityPos(tile.x, tile.y);

		return new SweeperAlienEntity {pos.x, pos.y, (int) evolution};
	}

	if (alien == Alien::VERTICAL) {
		glm::ivec2 tile = segment.getRandomPos(margin);
		glm::vec2 pos = toEntityPos(tile.x, tile.y);

		return new VerticalAlienEntity {pos.x, pos.y, (int) evolution};
	}

	if (alien == Alien::MINE) {
		glm::ivec2 tile = segment.getRandomPos(margin);
		glm::vec2 pos = toEntityPos(tile.x, tile.y);

		return new MineAlienEntity {pos.x, pos.y, (int) evolution};
	}

	if (alien == Alien::FIGHTER) {
		glm::ivec2 tile = segment.getRandomPos(margin);
		glm::vec2 pos = toEntityPos(tile.x, tile.y);

		return new FighterAlienEntity {pos.x, pos.y, (int) evolution};
	}

	if (alien == Alien::TURRET) {
		glm::ivec2 tile = segment.getRandomTurretPos(margin);
		glm::vec2 pos = toEntityPos(tile.x, tile.y);

		// failed to find valid turret placement
		if (tile.x == 0 && tile.y == 0) {
			return randomAlien(margin, segment);
		}

		return new TurretAlienEntity {pos.x, pos.y, (int) evolution};
	}

	if (alien == Alien::TESLA) {
		glm::ivec2 tile = segment.getRandomTeslaLeftPos(margin);

		glm::vec2 pos = toEntityPos(tile.x, tile.y);

		// failed to find valid turret placement
		if (tile.x == 0 && tile.y == 0) {
			return randomAlien(margin, segment);
		}

		glm::ivec2 tile2 = segment.getRandomTeslaRightPos(margin, tile);
		glm::vec2 pos2 = toEntityPos(tile2.x, tile2.y);

		auto left = addEntity(new TeslaAlienEntity(pos.x, pos.y, (int) evolution, TeslaAlienEntity::LEFT));
		auto right = addEntity(new TeslaAlienEntity(pos2.x, pos2.y, (int) evolution, TeslaAlienEntity::RIGHT));
		new RayBeamEntity(left, right);
	}

	return randomAlien(margin, segment);
}

std::shared_ptr<PlayerEntity> Level::getPlayer() {
	return player;
}

void Level::tick() {

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

			// try adding enemies
			for (int i = 0; i < count; i ++) {
				Entity* alien = randomAlien(1, segment);

				if (alien) {
					if (!alien->checkPlacement(*this)) {
						delete alien;
						continue;
					}

					addEntity(alien);
				}
			}

			// place powerups
			while (randomInt(0, manager.getPowerUpRarity()) == 0) {
				glm::ivec2 tile = segment.getRandomPos(6);
				glm::vec2 entity = toEntityPos(tile.x, tile.y);

				addEntity(new PowerUpEntity(entity.x, entity.y, PowerUpEntity::randomPick()));
			}

			// prepare for the next segment
			if (state != GameState::DEAD) {
				manager.tick(++total);
			}
		}
	}

	for (auto& entity : entities) {
		entity->tick(*this);
	}

	entities.erase(std::remove_if(entities.begin(), entities.end(), [] (const auto& entity) {
		return entity->shouldRemove();
	}), entities.end());

	for (auto& entity : pending) {
		entities.emplace_back(entity);

		if (std::shared_ptr<PlayerEntity> shared_player = std::dynamic_pointer_cast<PlayerEntity>(entity)) {
			player = shared_player;
		}
	}

	pending.clear();

	if (timer > 0) {
		timer --;

		if (timer == 0) {
			konami = 0;
		}
	}

	if (Input::isPressed(Key::UP) && (konami == 0)) {
		konami = 1;
		timer = 60 * 4;
	} else if (Input::isPressed(Key::UP) && (konami == 1)) konami = 2;
	else if (Input::isPressed(Key::DOWN) && (konami == 2)) konami = 3;
	else if (Input::isPressed(Key::DOWN) && (konami == 3)) konami = 4;
	else if (Input::isPressed(Key::LEFT) && (konami == 4)) konami = 5;
	else if (Input::isPressed(Key::RIGHT) && (konami == 5)) konami = 6;
	else if (Input::isPressed(Key::LEFT) && (konami == 6)) konami = 7;
	else if (Input::isPressed(Key::RIGHT) && (konami == 7)) konami = 8;
	else if (Input::isPressed(Key::B) && (konami == 8)) konami = 9;
	else if (Input::isPressed(Key::A) && (konami == 9)) {
		konami = 0;
		debug = !debug;
	}

	// player input
	if (state != GameState::DEAD) {

		if (Input::isPressed(Key::UP) || Input::isPressed(Key::W)) {
			skip += skip * 0.1 + 0.02;
		}

		if (skip > 1) skip = 1;
	} else {
		if (Input::isPressed(Key::ENTER)) {
			reload = true;
		}
	}
}

void Level::draw(TileSet& font8x8, BufferWriter<Vert4f4b>& text_writer, TileSet& tileset, BufferWriter<Vert4f4b>& game_writer) {

	for (auto& segment : segments) {
		segment.draw(scroll, tileset, game_writer);
	}

	for (auto& entity : entities) {
		entity->draw(*this, tileset, game_writer);
	}

	if (debug) {
		for (auto& entity : entities) {
			entity->debugDraw(*this, tileset, game_writer);
		}

		emitTextQuads(text_writer, 16, SH - 64, 20, 16, font8x8, 255, 255, 0, 220, "Seg: " + std::to_string(total), TextMode::LEFT);
		emitTextQuads(text_writer, 16, SH - 96, 20, 16, font8x8, 255, 255, 0, 220, "Bio: " + std::to_string(manager.getBiomeIndex()), TextMode::LEFT);
	}

	if (state != GameState::DEAD) {
		emitTextQuads(text_writer, SW - 32, SH - 32, 24, 20, font8x8, 255, 255, 0, 220, std::to_string(score), TextMode::RIGHT);
	}

	if (state == GameState::DEAD && (age % 120 < 60)) {
		if (age % 120 == 0) {
			SoundSystem::getInstance().add(Sounds::beep).play();
		}

		std::string over = "GAME OVER";

		int spacing = 8;
		int width = 48 + spacing;
		int start = (SW - (over.length() - 1) * width) / 2;

		emitTextQuads(text_writer, start, SH / 2 + 24, width, 48, font8x8, 255, 255, 0, 220, over, TextMode::LEFT);
		emitTextQuads(text_writer, start - 8, SH / 2 + 16 - 48, 24, 20, font8x8, 255, 255, 0, 220, "SCORE: " + std::to_string(score), TextMode::LEFT);

		if ((score > hi) && (age % 10 < 5)) {
			emitTextQuads(text_writer, start - 8, SH / 2 + 16 - 48 - 32, 24, 20, font8x8, 255, 255, 0, 220, "NEW HI-SCORE!", TextMode::LEFT);
		}
	}

}

glm::vec2 Level::toTilePos(int x, int y) const {
	const float pixels = SW / segment_width;
	return glm::vec2 {x, y} / pixels;
}

glm::vec2 Level::toEntityPos(int x, int y) const {
	const float pixels = SW / segment_width;
	return (glm::vec2 {x, y} + 0.5f) * pixels;
}

uint8_t Level::getTile(int tx, int ty) const {
	if (tx < 0 || tx >= segment_width) {
		return 0;
	}

	for (const auto& segment : segments) {
		if (segment.contains(ty)) {
			return segment.atWorldPos(tx, ty);
		}
	}

	return 0;
}

void Level::setTile(int tx, int ty, uint8_t tile) {
	if (tx < 0 || tx >= segment_width) {
		return;
	}

	for (auto& segment : segments) {
		if (segment.contains(ty)) {
			segment.atWorldPos(tx, ty) = tile;
		}
	}
}

float Level::getSkip() const {
	return skip;
}

float Level::getScroll() const {
	return scroll;
}

float Level::getSpeed() const {
	float v = std::min(1.0f, total * 0.01f);
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

	// no collision found
	return {};

}

Collision Level::checkCollision(Entity* self) {

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