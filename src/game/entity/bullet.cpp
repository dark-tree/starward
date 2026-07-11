
#include "bullet.hpp"

#include <game/emitter.hpp>

#include "enemy/alien.hpp"
#include "particle/blow.hpp"
#include "particle/tile.hpp"
#include "game/level/level.hpp"
#include "game/sounds.hpp"
#include "particle/dust.hpp"

// no 2d cross? Skill issue
static float cross(glm::vec2 v1, glm::vec2 v2) {
	return (v1.x*v2.y) - (v1.y*v2.x);
}

/*
 * BulletEntity
 */

BulletEntity::BulletEntity(float velocity, double x, double y, const std::shared_ptr<Entity>& parent, float angle, BulletConfig config)
: Entity(8, x, y) {
	this->parent = parent;
	this->velocity = velocity;
	this->angle = angle;
	this->time = 60 * 6;
	this->config = config;
	this->color = parent->isCausedByPlayer() ? Color::blue(config.charged) : Color::red(config.charged);
}

bool BulletEntity::isCharged() const {
	return getConfig().charged;
}

BulletConfig BulletEntity::getConfig() const {
	return config;
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

int BulletEntity::eraseTilesAround(Level& level, int radius) {
	glm::ivec2 pos = level.toTilePos(x, y);
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
		level.addEntity(new TileEntity(tile, vec.x, vec.y));
	}

	return broken.size();
}

bool BulletEntity::lockOntoTarget(Level& level) {
	float dist = std::numeric_limits<float>::infinity();
	std::shared_ptr<Entity> target = nullptr;

	for (auto e : level.getEntities()) {

		if (!e->shouldAutoTarget()) {
			continue;
		}

		if (isCausedByPlayer() == e->isCausedByPlayer()) {
			continue;
		}

		float horizontal = std::abs(e->x - x);

		if (horizontal < dist) {
			dist = horizontal;
			target = e;
		}
	}

	if (target) {
		this->target = target;
		return true;
	}

	return false;
}

void BulletEntity::tick(Level& level) {

	auto vec = velocity * glm::vec2(
		cos(deg(270) - angle),
		sin(deg(270) - angle)
	);

	x += vec.x;
	y += vec.y;

	if (target) {
		if (target->isDead()) {
			target = nullptr;
		} else {
			const glm::vec2 direction = glm::normalize(glm::vec2(target->x, target->y) - glm::vec2(x, y));
			const float correction = atan2(cross(vec, direction), dot(vec, direction)) * 0.1;
			angle -= correction;
		}
	} else if (config.targeting) {
		if (lockOntoTarget(level)) {
			level.addEntity(new DustEntity {x, y, 0, 0, 1, 1, 1, 30, Color::white().withAlpha(100)});
		}
	}

	if (time <= 0) {
		dead = true;
	} else {
		time --;
	}

	if (config.charged) {
		level.addEntity(new DustEntity {x, y, 0, 0, 1, 1, 1, 30, Color::white().withAlpha(100)});
	}

	if (config.boring && bored_tiles <= 16) {
		bored_tiles += eraseTilesAround(level, 2);
	}

	bool collided = false;
	Collision collision = level.checkCollision(this);

	// once we hit an entity with piercing we wait for some time
	// before we can do that again (to not one-shot aliens)
	// but we still need to collide with terrain
	if (cooldown > 0) {
		cooldown --;
	}

	if (collision.type == Collision::ENTITY && cooldown == 0) {

		if (collision.entity) {
			collision.entity->onDamage(level, 1, this);
		}

		if (config.piercing) {
			cooldown = 15;
		} else {
			dead = true;
		}

		collided = true;
	}

	if (collision.type == Collision::TILE) {

		int radius = isCausedByPlayer() ? 5 : 4;
		eraseTilesAround(level, radius);

		dead = true;
		collided = true;
	}

	if (collided) {
		level.addEntity(new BlowEntity(x, y));
		SoundSystem::getInstance().add(Sounds::damage).play();
	}

	Entity::tick(level);
}

void BulletEntity::draw(Level& level, Renderer& renderer) {
	float alpha = (time / 10.0f);

	if (alpha > 1) {
		alpha = 1;
	}

	if (level.isDebug() && target) {
		emitLineQuad(*renderer.terrain.writer, x, y + level.getScroll(), target->x, target->y + level.getScroll() - 16, 2, renderer.terrain.tileset->sprite(0, 0), color.r, color.g, color.b, 100);
	}

	Sprite sprite = renderer.terrain.tileset->sprite(config.piercing ? 7 : 6, 5);
	emitEntityQuad(level, *renderer.terrain.writer, sprite, 16, angle, color.withAlpha(alpha * 255));
}

bool BulletEntity::shouldAutoTarget() {
	return false;
}
