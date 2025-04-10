#include "level.hpp"
#include "sounds.hpp"
#include "emitter.hpp"
#include "biome.hpp"
#include "entity/all.hpp"

int global_segment_id = 0;

Level::Level(BiomeManager& manager)
: manager(manager) {
	std::string hi_str = platform_read_string("hi");

	if (hi_str.length() > 0) {
		hi = std::stoi(hi_str);
	}

	manager.tick(0);
}

void Level::reset() {
	printf("Resetting level...\n");

	// we need to do this part manually
	for (Entity* entity : pending) {
		delete entity;
	}

	// reset world gen
	manager.reset();
	global_segment_id = 0;

	// recreate level
	this->~Level();
	new (this) Level(manager);

	// respawn player
	spawnInitial();
}

void Level::spawnInitial() {
	addEntity(new PlayerEntity {});
	addEntity(new SweeperAlienEntity {100, 450, 0});
	addEntity(new PowerUpEntity {200, 600, PowerUpEntity::LIVE});
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

void Level::addEntity(Entity* entity) {
	pending.emplace_back(entity);
}

Entity* Level::randomAlien(int margin, LevelSegment& segment) {
	Alien alien = manager.getAlien();
	Evolution evolution = manager.getEvolution();

	if (alien == Alien::SWEEPER) {
		glm::ivec2 tile = segment.getRandomPos(margin);
		glm::vec2 pos = toEntityPos(tile.x, tile.y);

		return new SweeperAlienEntity {pos.x, pos.y, (int) evolution};
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
	} else {
		age = 60;
		aliveness = 1.0f;
	}

	biome_speed = manager.getBonusSpeed();
	scroll -= getSpeed();
	skip *= 0.95;

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

	for (Entity* entity : pending) {
		std::shared_ptr<Entity>& ptr = entities.emplace_back(entity);

		if (std::shared_ptr<PlayerEntity> shared_player = std::dynamic_pointer_cast<PlayerEntity>(ptr)) {
			player = shared_player;
		}
	}

	pending.clear();

	// player input
	if (state != GameState::DEAD) {
		if (gls::Input::is_pressed(Key::UP) || gls::Input::is_pressed(Key::W)) {
			skip += skip * 0.1 + 0.02;
		}

		if (skip > 1) skip = 1;
	} else {
		if (gls::Input::is_pressed(Key::ENTER)) {
			this->reset();
		}
	}
}

void Level::draw(gls::TileSet& font8x8, gls::BufferWriter<gls::Vert4f4b>& text_writer, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& game_writer) {

	for (auto& segment : segments) {
		segment.draw(scroll, tileset, game_writer);
	}

	for (auto& entity : entities) {
		entity->draw(*this, tileset, game_writer);
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

glm::ivec2 Level::toTilePos(int x, int y) {
	const float pixels = SW / segment_width;
	return glm::vec2 {x, y} / pixels;
}

glm::vec2 Level::toEntityPos(int x, int y) {
	const float pixels = SW / segment_width;
	return (glm::vec2 {x, y} + 0.5f) * pixels;
}

uint8_t Level::get(int tx, int ty) {
	if (tx < 0 || tx >= segment_width) {
		return 0;
	}

	for (auto& segment : segments) {
		if (segment.contains(ty)) {
			return segment.atWorldPos(tx, ty);
		}
	}

	return 0;
}

void Level::set(int tx, int ty, uint8_t tile) {
	if (tx < 0 || tx >= segment_width) {
		return;
	}

	for (auto& segment : segments) {
		if (segment.contains(ty)) {
			segment.atWorldPos(tx, ty) = tile;
		}
	}
}

double Level::getSkip() const {
	return skip;
}

double Level::getScroll() const {
	return scroll;
}

double Level::getSpeed() const {
	float v = std::min(1.0f, total * 0.01f);
	return base_speed * std::max(aliveness, 0.5f) + (skip * 4 + v * 2 + biome_speed) * aliveness;
}

Collision Level::checkCollision(Entity* self) {
	float x = self->x;
	float y = self->y;
	float size = self->size;

	if (x + size < 0 || x - size > SW) {
		return {};
	}

	float pixels = SW / segment_width;

	int radius = self->tile_radius;
	int tx = round(x / pixels);
	int ty = round(y / pixels);

	for (int ox = - radius; ox <= radius; ox ++) {
		for (int oy = - radius; oy <= radius; oy ++) {
			uint8_t tile = get(tx + ox, ty + oy);

			if (tile) {
				return {tx + ox, ty + oy};
			}
		}
	}

	for (auto& entity : entities) {
		Entity* pointer = entity.get();

		if (pointer == self) {
			continue;
		}

		if (entity->shouldCollide(self)) {
			return {pointer};
		}
	}

	return {};
}

std::vector<std::shared_ptr<Entity>>& Level::getEntities() {
	return entities;
}