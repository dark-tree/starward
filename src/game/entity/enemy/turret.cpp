
#include "turret.hpp"

#include "game/entity/bullet.hpp"
#include "game/entity/particle/dust.hpp"
#include "game/level/level.hpp"

/*
 * TurretAlienEntity
 */

TurretAlienEntity::TurretAlienEntity(double x, double y, int evolution)
	: AlienEntity(x, y, evolution) {
}

bool TurretAlienEntity::checkPlacement(Level& level) {
	Collision collision = level.checkCollision(this);

	if (collision.type == Collision::ENTITY) {
		return false;
	}

	return true;
}

void TurretAlienEntity::shoot(Level& level, float speed, float radius, float offset) {

	// calculate shooting position
	float angle = deg(90) - head + offset;
	float ax = cos(angle);
	float ay = sin(angle);

	float effect = radius - 8;

	// create bullet
	level.addEntity(new BulletEntity {-speed, x + radius * ax, y + radius * ay, self(), head});

	// particle effect
	for (int i = randomInt(2, 5); i > 0; i--) {
		level.addEntity(new DustEntity {x + effect * ax, y + effect * ay, ax, ay, head, 1, 0.5, 20, Color::red()});
	}

}

void TurretAlienEntity::tick(Level& level) {
	AlienEntity::tick(level);

	if (std::shared_ptr<PlayerEntity> player = level.getPlayer()) {
		glm::vec2 dir {player->x - x, player->y - y};
		float bullet = 5;
		float radius = 32;

		// try to be sneaky and target future player position
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

void TurretAlienEntity::draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) {
	const Color color = Color::red(damage_ticks);

	emitEntityQuad(level, writer, tileset.sprite(0, 6), size, angle, color);
	emitEntityQuad(level, writer, tileset.sprite(evolution + 1, 6), size, head, color);
}