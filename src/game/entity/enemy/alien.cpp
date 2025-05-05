
#include "alien.hpp"

#include "game/level/level.hpp"
#include "game/entity/particle/dust.hpp"

/*
 * AlienEntity
 */

AlienEntity::AlienEntity(float x, float y, int evolution)
: Entity(32, x, y) {
	this->evolution = evolution;
}

void AlienEntity::spawnParticles(Level& level, int min, int max, float ovx, float ovy) {
	for (int i = randomInt(min, max); i > 0; i--) {
		float vx = randomFloat(-1, 1) + ovx;
		float vy = randomFloat(-1, 1) + ovy;

		level.addEntity(new DustEntity {x, y, vx, vy, 1, 1, 1, 30, Color::red()});
	}
}

bool AlienEntity::wasAttacked() const {
	return attacked;
}

void AlienEntity::onDamage(Level& level, int damage, NULLABLE Entity* damager) {
	if (damage <= 0) {
		this->damage_ticks = 4;
		return;
	}

	if (damager && damager->isCausedByPlayer()) {

		this->attacked = true;
		this->health --;
		this->damage_ticks = 4;

		onDamaged(level);

		if (health <= 0) {
			this->dead = true;
			onKilled(level);
		}

	}
}

void AlienEntity::tick(Level& level) {
	Entity::tick(level);

	if (damage_ticks) {
		damage_ticks --;
	}
}

void AlienEntity::onDamaged(Level& level) {
}

void AlienEntity::onKilled(Level& level) {
	spawnParticles(level, 5, 10);
	level.addScore(100);
}
