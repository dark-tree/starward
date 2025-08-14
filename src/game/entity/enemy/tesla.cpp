#include "tesla.hpp"

#include "game/level/level.hpp"

/*
 * TeslaAlienEntity
 */

void TeslaAlienEntity::generateFoundation(Segment& segment, glm::ivec2 pos, int x, bool flip) {
	const int count = 7;
	int heights[count]  = {2, 3, 3, 2, 2, 3, 3};
	int material[count] = {2, 2, 2, 0, 0, 0, 0};

	for (int i = 0; i < count; i ++) {

		int j = flip ? count - i - 1 : i;
		int h = heights[j];
		int m = material[j];

		for (int y = -h; y <= h; y ++) {
			int tx = pos.x + x;
			int ty = pos.y + y - segment.getVerticalOffset();

			if (tx < 0 || tx >= Segment::width || ty < 0 || ty >= Segment::height) {
				continue;
			}

			segment.at(tx, ty) = m;
		}

		x ++;
	}
}

TeslaAlienEntity::TeslaAlienEntity(double x, double y, int evolution, Side side)
	: AlienEntity(x, y, evolution), side(side) {
	this->health = 3;

	if (side == RIGHT) {
		this->collider = Box {-10, -20, 40, 40};
		this->angle = 0;
	} else {
		this->collider = Box {-30, -20, 40, 40};
		this->angle = M_PI;
	}
}

bool TeslaAlienEntity::checkPlacement(Level& level) {
	Collision collision = level.checkCollision(this);

	if (collision.type == Collision::ENTITY) {
		return false;
	}

	return true;
}

void TeslaAlienEntity::tick(Level& level) {
	AlienEntity::tick(level);
}

void TeslaAlienEntity::draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) {
	const Color color = Color::red(damage_ticks);

	emitEntityQuad(level, writer, tileset.sprite(1, 13), size, angle, color);
}

void TeslaAlienEntity::onSpawned(const Level& level, Segment* segment) {

	int xs[] = {-4, -2};
	glm::ivec2 pos = level.toTilePos(x, y);

	if (segment) {
		generateFoundation(*segment, pos, xs[side], side == RIGHT);
	}
}
