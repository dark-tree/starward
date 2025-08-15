
#include "bullet.hpp"

#include "particle/blow.hpp"
#include "particle/tile.hpp"
#include "game/level/level.hpp"
#include "game/sounds.hpp"

/*
 * BulletEntity
 */

BulletEntity::BulletEntity(float velocity, double x, double y, const std::shared_ptr<Entity>& parent, float angle)
: Entity(8, x, y) {
	this->parent = parent;
	this->velocity = velocity;
	this->angle = angle;
	this->time = 60 * 6;
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
		if (level.getTile(point.x, point.y) == 2) {
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

	if (time <= 0) {
		dead = true;
	} else {
		time --;
	}

	Collision collision = level.checkCollision(this);

	if (collision.type != Collision::MISS) {
		level.addEntity(new BlowEntity(x, y));
		SoundSystem::getInstance().add(Sounds::damage).play();

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

					uint8_t tile = level.getTile(tx, ty);

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
			uint8_t tile = level.getTile(pos.x, pos.y);

			level.setTile(pos.x, pos.y, 0);
			level.addEntity(new TileEntity(x, y, tile, vec.x, vec.y));
		}

		dead = true;
	}

	Entity::tick(level);
}

void BulletEntity::draw(Level& level, Renderer& renderer) {
	Color color = velocity < 0 ? Color::red() : Color::blue();
	float alpha = (time / 10.0f);

	if (alpha > 1) {
		alpha = 1;
	}

	emitEntityQuad(level, *renderer.terrain.writer, renderer.terrain.tileset->sprite(0, 0), size, angle, color.withAlpha(alpha * 255));
}
