
#include "sweeper.hpp"
#include "bullet.hpp"
#include "blow.hpp"
#include "dust.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
#include "game/emitter.hpp"
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

		float bx = x;

		if (evolution >= 2) {
			bx += (count % 2 == 1 ? -size : size) * 0.3f;
		}

		if (visible) {
			level.addEntity(new BulletEntity{-3, bx, y - 24, self()});
		}
	}

	AlienEntity::tick(level);
}

void SweeperAlienEntity::draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) {
	emitEntityQuad(level, writer, tileset.sprite(evolution, 5), size, angle, Color::red(damage_ticks || (buried % 10 > 5)));
}
