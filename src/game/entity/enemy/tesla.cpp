#include "tesla.hpp"

#include "ray.hpp"
#include "game/level/level.hpp"
#include "game/level/matcher.hpp"

/*
 * TeslaAlienEntity
 */

bool TeslaAlienEntity::spawnAt(Level& level, int lx, int rx, int y, int evolution) {

	glm::vec2 lp = Level::toEntityPos(lx, y);
	glm::vec2 rp = Level::toEntityPos(rx, y);

	std::shared_ptr<TeslaAlienEntity> left = std::make_shared<TeslaAlienEntity>(lp.x, lp.y, evolution, LEFT);
	std::shared_ptr<TeslaAlienEntity> right = std::make_shared<TeslaAlienEntity>(rp.x, rp.y, evolution, RIGHT);

	// manually check placement so that both need to match first
	if (!left->checkPlacement(level) || !right->checkPlacement(level)){
		return false;
	}

	level.addEntity(left);
	level.addEntity(right);
	level.addEntity(new RayBeamEntity(left, right));
	return true;
}

bool TeslaAlienEntity::spawn(Level& level, Segment& segment, int evolution) {

	int rows[Segment::height];
	randomBuffer(rows, Segment::height);

	for (int row = 0; row < Segment::height; row++) {
		int cols[Segment::width];
		randomBuffer(cols, Segment::width);

		for (int col = 0; col < Segment::width; col++) {
			TerrainMacher matcher{segment, cols[col], rows[row]};

			if (!matcher.acceptRight(true, 5, 20)) {
				continue;
			}

			glm::ivec2 left = matcher.here();

			if (!matcher.acceptRight(false, 15, 100)) {
				continue;
			}

			glm::ivec2 right = matcher.here();

			if (!matcher.acceptRight(true, 5, 20)) {
				continue;
			}

			return spawnAt(level, left.x, right.x, left.y + segment.getVerticalOffset(), evolution);
		}
	}

	printf("Failed to find tesla tower placement spot!\n");
	return false;
}

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

void TeslaAlienEntity::draw(Level& level, Renderer& renderer) {
	const Color color = Color::red(damage_ticks);

	emitEntityQuad(level, *renderer.terrain.writer, renderer.terrain.tileset->sprite(1, 13), size, angle, color);
}

void TeslaAlienEntity::onSpawned(const Level& level, Segment* segment) {

	int xs[] = {-4, -2};
	glm::ivec2 pos = level.toTilePos(x, y);

	if (segment) {
		generateFoundation(*segment, pos, xs[side], side == RIGHT);
	}
}
