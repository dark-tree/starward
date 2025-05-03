#include "vertical.hpp"

#include <game/level.hpp>
#include "bullet.hpp"

/*
 * VerticalAlienEntity
 */

Box VerticalAlienEntity::getBoxTrigger() const {
	return trigger.withOffset(x, y);
}

VerticalAlienEntity::VerticalAlienEntity(double x, double y, int evolution)
: SweeperAlienEntity(x, y, evolution) {
	this->trigger = Box {-32, -300, 64, 300};
	this->facing = -1;
	this->active = false;
}

void VerticalAlienEntity::tick(Level& level) {

	this->active = false;

	for (auto& entity : level.getEntities()) {
		if (entity->getBoxCollider().intersects(getBoxTrigger())) {
			if (entity->isCausedByPlayer()) {
				this->active = true;
				break;
			}
		}
	}

	if (active && (age % 30 == 0)) {
		float bx = x;

		if (evolution >= 2) {
			bx += (count % 2 == 1 ? -size : size) * 0.3f;
		}

		level.addEntity(new BulletEntity{-3, bx, y - 24, self()});
	}

	SweeperAlienEntity::tick(level);
}

void VerticalAlienEntity::draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) {
	emitEntityQuad(level, writer, tileset.sprite(evolution, 8), size, angle, Color::red(damage_ticks || (buried % 10 > 5)));
}

inline void VerticalAlienEntity::debugDraw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) {
	AlienEntity::debugDraw(level, tileset, writer);
	emitBoxWireframe(getBoxTrigger().withOffset(0, level.getScroll()), writer, tileset.sprite(0, 0), 1, Color::white());
}

void VerticalAlienEntity::tickMovement() {
	const float speed = evolution > 1 ? 2 : 1;
	this->y += facing * speed + bump;
}

void VerticalAlienEntity::tickShooting(Level& level) {
	level.addEntity(new BulletEntity{-3, x + 32, y, self(), + M_PI_2});
	level.addEntity(new BulletEntity{-3, x - 32, y, self(), - M_PI_2});
}
