
#include "sweeper.hpp"

#include "game/entity/bullet.hpp"
#include "game/entity/particle/blow.hpp"
#include "game/level/level.hpp"
#include "game/sounds.hpp"

/*
 * SweeperAlienEntity
 */

SweeperAlienEntity::SweeperAlienEntity(double x, double y, int evolution)
: AlienEntity(x, y, evolution) {
	this->health += 2 + evolution;
}

bool SweeperAlienEntity::checkPlacement(Level& level) {
	return level.checkCollision(this).type == Collision::MISS;
}

void SweeperAlienEntity::onDamaged(Level& level) {
	AlienEntity::onDamaged(level);

	bump = 4;
}

void SweeperAlienEntity::onDamage(Level& level, int damage, Entity* damager) {
	AlienEntity::onDamage(level, damage, damager);

	if (damager && !damager->isCausedByPlayer()) {
		facing *= -1;
	}
}

void SweeperAlienEntity::tick(Level& level) {

	if (stan_ticks > 0) {
		bump = 0;
		AlienEntity::tick(level);
		return;
	}

	tickMovement();

	if (this->bump > 0) {
		this->bump -= 0.1;
	} else {
		this->bump = 0;
	}

	this->cooldown -= 0.05f;

	Collision collision = level.checkCollision(this);

	if (collision.type == Collision::MISS) {
		buried = 0;
	}

	if (collision.type == Collision::TILE) {
		buried ++;

		if (buried > 120) {
			dead = true;
			level.addEntity(new BlowEntity(x, y));
			SoundSystem::getInstance().add(Sounds::blow).play();

			if (wasAttacked()) {
				level.addScore(200);
			}

			return;
		}
	}

	if ((x < 0) || (x > SW) || collision.type != Collision::MISS) {
		facing *= -1;
	}

	if (cooldown <= 0) {
		cooldown = 0.75;
		count ++;

		if (count >= health) {
			count = 0;
			cooldown = 5 - evolution;
		}

		if (visible) {
			tickShooting(level);
		}
	}

	AlienEntity::tick(level);
}

void SweeperAlienEntity::draw(Level& level, Renderer& renderer) {
	emitEntityQuad(level, *renderer.terrain.writer, renderer.terrain.tileset->sprite(evolution, 5), size, angle + facing * 0.05, Color::red(damage_ticks || (buried % 10 > 5)));
}

void SweeperAlienEntity::tickShooting(Level& level) {
	float bx = x;

	if (evolution >= 2) {
		bx += (count % 2 == 1 ? -size : size) * 0.3f;
	}

	level.addEntity(new BulletEntity{-3, bx, y - 24, self(), M_PI});
}

void SweeperAlienEntity::tickMovement() {
	const float speed = evolution > 1 ? 2 : 1;
	this->x += facing * speed;
	this->y += bump;
}
