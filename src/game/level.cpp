#include "level.hpp"
#include "sounds.hpp"

void Level::addEntity(Entity* entity) {
	pending.push_back(entity);
}

void Level::tick() {

	scroll -= 1 + skip * 4;
	skip *= 0.95;

	for (auto& segment : segments) {
		segment.tick(scroll);
	}

	for (auto& entity : entities) {
		entity->tick(*this);
	}

	entities.erase(std::remove_if(entities.begin(), entities.end(), [] (const auto& entity) {
		return entity->shouldRemove();
	}), entities.end());

	for (Entity* entity : pending) {
		entities.emplace_back(entity);
	}

	pending.clear();

	// player input

	if (gls::Input::is_pressed(Key::UP)) {
		skip += skip * 0.1 + 0.02;
	}

	if (skip > 1) skip = 1;

}

void Level::draw(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) {

	for (auto& segment : segments) {
		segment.draw(scroll, tileset, buffer);
	}

	for (auto& entity : entities) {
		entity->draw(*this, tileset, buffer);
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
	if (tx < 0 || tx > segment_width) {
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
	if (tx < 0 || tx > segment_width) {
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

bool Level::checkCollision(Entity* self, Entity* except) {
	float x = self->x;
	float y = self->y;
	float size = self->size;

	if (x + size < 0 || x - size > SW) {
		return false;
	}

	float pixels = SW / segment_width;

	int radius = 1;
	int tx = round(x / pixels);
	int ty = round(y / pixels);

	for (int ox = - radius; ox <= radius; ox ++) {
		for (int oy = - radius; oy <= radius; oy ++) {
			uint8_t tile = get(tx + ox, ty + oy);

			if (tile) {
				return true;
			}
		}
	}

	for (auto& entity : entities) {
		Entity* pointer = entity.get();

		if ((pointer == except) || (pointer == self)) {
			continue;
		}

		if (entity->checkCollision(x, self->y, size)) {
			return true;
		}
	}

	return false;
}
