#include "entity.hpp"
#include "level.hpp"
#include "sounds.hpp"

/*
 * Entity
 */

Entity::Entity(double size, double x, double y)
: size(size) {
	this->x = x;
	this->y = y;
}

Entity::~Entity() {}

bool Entity::shouldRemove() const {
	return dead;
}

bool Entity::checkCollision(float x, float y, float size) {
	float dx = this->x - x;
	float dy = this->y - y;

	return (sqrt(dx * dx + dy * dy) < (size + this->size));
}

gls::Sprite Entity::sprite(gls::TileSet& tileset) {
	return tileset.sprite(0, 0);
}

void Entity::draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) {
	gls::Sprite s = sprite(tileset);

	const float tx = x - (size / 2);
	const float ty = y + level.getScroll() - (size / 2);
	const float w = size;
	const float h = size;

	writer.push({tx + 0, 0 + ty, s.min_u, s.min_v, 0, 0, 255, 255});
	writer.push({tx + w, 0 + ty, s.max_u, s.min_v, 0, 0, 255, 255});
	writer.push({tx + w, h + ty, s.max_u, s.max_v, 0, 0, 255, 255});
	writer.push({tx + w, h + ty, s.max_u, s.max_v, 0, 0, 255, 255});
	writer.push({tx + 0, h + ty, s.min_u, s.max_v, 0, 0, 255, 255});
	writer.push({tx + 0, 0 + ty, s.min_u, s.min_v, 0, 0, 255, 255});
}

void Entity::tick(Level& level) {
	if (level.getScroll() + y < -size) {
		dead = true;
	}

	age += 1;
}

/*
 * BulletEntity
 */

BulletEntity::BulletEntity(float velocity, double x, double y, Entity* parent)
: Entity(8, x, y) {
	this->parent = parent;
	this->velocity = velocity;
}

void BulletEntity::tick(Level& level) {
	y += velocity;

	if (age > 60 * 4) {
		dead = true;
	}

	if (level.checkCollision(this, parent)) {
		level.addEntity(new BlowEntity(x, y));
		SoundSystem::getInstance().add(Sounds::hit_1).play();

		glm::ivec2 pos = level.toTilePos(x, y);
		int radius = 4;

		for (int ox = -radius; ox <= radius; ox ++) {
			for (int oy = -radius; oy <= radius; oy ++) {
				if (sqrt(ox * ox + oy * oy) < radius) {
					level.set(pos.x + ox, pos.y + oy, 0);
				}
			}
		}

		dead = true;
	}

	Entity::tick(level);
}

/*
 * BlowEntity
 */

BlowEntity::BlowEntity(double x, double y)
: Entity(64, x, y) {}

bool BlowEntity::checkCollision(float x, float y, float size) {
	return false;
}

gls::Sprite BlowEntity::sprite(gls::TileSet& tileset) {
	return tileset.sprite((int) age / 5, 2);
}

void BlowEntity::tick(Level& level) {
	Entity::tick(level);

	if (age >= 5 * 4) {
		dead = true;
	}
}

/*
 * PlayerEntity
 */

PlayerEntity::PlayerEntity()
: Entity(64, SW / 2, 0) {}

gls::Sprite PlayerEntity::sprite(gls::TileSet& tileset) {
	return tileset.sprite(2, 0);
}

void PlayerEntity::tick(Level& level) {

	this->y = 64 - level.getScroll() + level.getSkip() * 32;
	this->cooldown -= 0.1f;

	if (gls::Input::is_pressed(Key::LEFT)) {
		this->x -= 6;
	}

	if (gls::Input::is_pressed(Key::RIGHT)) {
		this->x += 6;
	}

	if ((cooldown <= 0) && gls::Input::is_pressed(Key::SPACE)) {
		level.addEntity(new BulletEntity {11, x, y + 32, this});
		SoundSystem::getInstance().add(Sounds::hit_0).play();
		cooldown = 1;
	}

}