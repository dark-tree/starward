
#include "bullet.hpp"
#include "blow.hpp"
#include "tile.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * BulletEntity
 */

BulletEntity::BulletEntity(float velocity, double x, double y, const std::shared_ptr<Entity>& parent, float angle)
: Entity(1, 8, x, y) {
	this->parent = parent;
	this->velocity = velocity;
	this->angle = angle;

	if (velocity < 0) {
		this->r = 255;
		this->g = 50;
		this->b = 50;
		this->a = 255;
	} else {
		this->r = 50;
		this->g = 50;
		this->b = 255;
		this->a = 255;
	}
}

bool BulletEntity::isCausedByPlayer() {
	return parent && parent->isCausedByPlayer();
}

std::shared_ptr<Entity> BulletEntity::getParent() {
	return parent;
}

bool BulletEntity::isTileProtected(Level& level, glm::ivec2 pos, int tx, int ty) {
	glm::ivec2 end {tx, ty};

	for (glm::ivec2 point : trace(pos, end)) {
		if (level.get(point.x, point.y) == 2) {
			if (point == pos || point == end) {
				continue;
			}

			return true;
		}
	}

	return false;
}

void BulletEntity::tick(Level& level) {

	x += velocity * cos(deg(270) - angle);
	y += velocity * sin(deg(270) - angle);

	if (age > 60 * 4) {
		dead = true;
	}

	Collision collision = level.checkCollision(this);

	if (collision.type != Collision::MISS) {
		level.addEntity(new BlowEntity(x, y));
		SoundSystem::getInstance().add(Sounds::getRandomDamage()).play();

		if (collision.entity) {
			collision.entity->onDamage(level, 1, this);
		}

		glm::ivec2 pos = level.toTilePos(x, y);
		int radius = isCausedByPlayer() ? 5 : 4;

		std::vector<glm::ivec2> broken;

		for (int ox = -radius; ox <= radius; ox ++) {
			for (int oy = -radius; oy <= radius; oy ++) {
				if (sqrt(ox * ox + oy * oy) < radius) {

					int tx = pos.x + ox;
					int ty = pos.y + oy;

					uint8_t tile = level.get(tx, ty);

					if (tile != 0) {
						if (!isTileProtected(level, pos, tx, ty)) {
							broken.emplace_back(tx, ty);
						}
					}
				}
			}
		}

		for (glm::ivec2 pos : broken) {
			glm::ivec2 vec = level.toEntityPos(pos.x, pos.y);
			uint8_t tile = level.get(pos.x, pos.y);

			level.set(pos.x, pos.y, 0);
			level.addEntity(new TileEntity(x, y, tile, vec.x, vec.y));
		}

		dead = true;
	}

	Entity::tick(level);
}