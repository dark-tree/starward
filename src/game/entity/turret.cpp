
#include "turret.hpp"
#include "bullet.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * TurretAlienEntity
 */

TurretAlienEntity::TurretAlienEntity(double x, double y, int evolution)
: Entity(2, 32, x, y) {
	this->evolution = evolution;
}

bool TurretAlienEntity::checkPlacement(Level& level) {
	Collision collision = level.checkCollision(this);

	if (collision.type == Collision::ENTITY) {
		return false;
	}

	return true;
}

void TurretAlienEntity::onDamage(Level& level, int damage, Entity* damager) {
	if (damager && damager->isCausedByPlayer()) {
		level.addScore(100);
		this->dead = true;
	}
}

void TurretAlienEntity::shoot(Level& level, float speed, float radius, float offset) {

	// calculate shooting position
	float angle = deg(90) - head + offset;
	float dx = radius * cos(angle);
	float dy = radius * sin(angle);

	// create bullet
	level.addEntity(new BulletEntity {-speed, x + dx, y + dy, self(), head});

}

void TurretAlienEntity::tick(Level& level) {
	Entity::tick(level);

	if (std::shared_ptr<PlayerEntity> player = level.getPlayer()) {
		glm::vec2 dir {player->x - x, player->y - y};
		float bullet = 5;
		float radius = 32;

		// try to be sneaky and target future position
		float oy = level.getSpeed() * (glm::length(dir) - radius) / bullet;
		target = deg(90) - std::atan2(dir.y + oy, dir.x);
		head = slerp(head, target, deg(1 + evolution));

		if (cooldown <= 0) {
			cooldown = 4.05f - evolution;

			if (evolution >= 2) {
				barrel = -barrel;
				cooldown /= 2;

				if (barrel == 1) {
					shoot(level, bullet, radius, -deg(20));
				} else {
					shoot(level, bullet, radius, +deg(20));
				}
			} else {
				shoot(level, bullet, radius, 0);
			}
		}

		this->cooldown -= 0.05f;
	}
}

void TurretAlienEntity::draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) {
	emitEntityQuad(level, writer, tileset.sprite(0, 6), size, angle, Color::red());
	emitEntityQuad(level, writer, tileset.sprite(evolution + 1, 6), size, head, Color::red());
}