#include "tesla.hpp"

#include "game/level/level.hpp"

/*
 * TeslaAlienEntity
 */

TeslaAlienEntity::TeslaAlienEntity(double x, double y, int evolution, Side side)
	: AlienEntity(x, y, evolution) {
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
