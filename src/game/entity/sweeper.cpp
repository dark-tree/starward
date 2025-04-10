
#include "sweeper.hpp"
#include "bullet.hpp"
#include "blow.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * SweeperAlienEntity
 */

SweeperAlienEntity::SweeperAlienEntity(double x, double y, int evolution)
: Entity(2, 32, x, y) {
	this->r = 255;
	this->g = 50;
	this->b = 50;
	this->a = 255;

	this->evolution = evolution;
	this->health += evolution;
}

bool SweeperAlienEntity::checkPlacement(Level& level) {
	return level.checkCollision(this).type == Collision::MISS;
}

void SweeperAlienEntity::onDamage(Level& level, int damage, Entity* damager) {
	if (damager && !damager->isCausedByPlayer()) {
		facing *= -1;
		return;
	}

	health --;
	bump = 4;
	attacked = true;

	if (health <= 0) {
		this->dead = true;

		if (damager && damager->isCausedByPlayer()) {
			level.addScore(100);
		}
	}
}

gls::Sprite SweeperAlienEntity::sprite(gls::TileSet& tileset) {
	return tileset.sprite(evolution, 5);
}

void SweeperAlienEntity::tick(Level& level) {

	float speed = evolution > 1 ? 2 : 1;

	move(level, facing * speed, bump);

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

			if (attacked) {
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

		float bx = x;

		if (evolution >= 2) {
			bx += (count % 2 == 1 ? -size : size) * 0.3f;
		}

		if (visible) {
			level.addEntity(new BulletEntity{-3, bx, y - 24, self()});
		}
	}

	Entity::tick(level);
}