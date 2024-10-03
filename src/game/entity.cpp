#include "entity.hpp"
#include "level.hpp"
#include "sounds.hpp"
#include "tile.hpp"

/*
 * Entity
 */

Entity::Entity(double size, double x, double y)
: size(size) {
	this->x = x;
	this->y = y;

	this->r = 255;
	this->g = 255;
	this->b = 255;
	this->a = 255;
}

Entity::~Entity() {}

bool Entity::shouldRemove() const {
	return dead;
}

bool Entity::shouldCollide(Entity* entity) {
	float ts = this->size / 2;
	float es = entity->size / 2;

	if (this->x + ts < entity->x - es || this->x - ts > entity->x + es) {
		return false;
	}

	if (this->y + ts < entity->y - es || this->y - ts > entity->y + es) {
		return false;
	}

	return true;
}

void Entity::onDamage(int damage) {
	this->dead = true;
}

gls::Sprite Entity::sprite(gls::TileSet& tileset) {
	return tileset.sprite(0, 0);
}

void Entity::draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) {
	gls::Sprite s = sprite(tileset);

	const float tx = x;
	const float ty = y + level.getScroll();

	double quarter = M_PI / 2;
	double start = M_PI / 4 + M_PI / 2;

	auto getUnitPoint = [=] (int index) {
		return glm::vec2 {sin(start + quarter * index), cos(start + quarter * index)} * (float) size;
	};

	glm::vec2 v0 = getUnitPoint(0);
	glm::vec2 v1 = getUnitPoint(1);
	glm::vec2 v2 = getUnitPoint(2);
	glm::vec2 v3 = getUnitPoint(3);

	writer.push({tx + v0.x, v0.y + ty, s.min_u, s.min_v, r, g, b, a});
	writer.push({tx + v1.x, v1.y + ty, s.max_u, s.min_v, r, g, b, a});
	writer.push({tx + v2.x, v2.y + ty, s.max_u, s.max_v, r, g, b, a});

	writer.push({tx + v2.x, v2.y + ty, s.max_u, s.max_v, r, g, b, a});
	writer.push({tx + v3.x, v3.y + ty, s.min_u, s.max_v, r, g, b, a});
	writer.push({tx + v0.x, v0.y + ty, s.min_u, s.min_v, r, g, b, a});

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

void BulletEntity::tick(Level& level) {
	y += velocity;

	if (age > 60 * 4) {
		dead = true;
	}

	Collision collision = level.checkCollision(this);

	if (collision.type != Collision::MISS) {
		level.addEntity(new BlowEntity(x, y));
		SoundSystem::getInstance().add(Sounds::hit_1).play();

		if (collision.entity) {
			collision.entity->onDamage(1);
		}

		glm::ivec2 pos = level.toTilePos(x, y);
		int radius = 5;

		for (int ox = -radius; ox <= radius; ox ++) {
			for (int oy = -radius; oy <= radius; oy ++) {
				if (sqrt(ox * ox + oy * oy) < radius) {

					int tx = pos.x + ox;
					int ty = pos.y + oy;

					uint8_t tile = level.get(tx, ty);
					glm::ivec2 vec = level.toEntityPos(tx, ty);

					if (tile) {
						level.set(tx, ty, 0);
						level.addEntity(new TileEntity(x, y, tile, vec.x, vec.y));
					}

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

bool BlowEntity::shouldCollide(Entity* entity) {
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
: Entity(64, SW / 2, 0) {
	this->r = 50;
	this->g = 50;
	this->b = 255;
	this->a = 255;
}

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
		level.addEntity(new BulletEntity {11, x, y + 64, this});
		SoundSystem::getInstance().add(Sounds::hit_0).play();
		cooldown = 1;
	}

	if (this->x < 0) {
		this->x = 0;
	}

	if (this->x > SW) {
		this->x = SW;
	}

}

/*
 * SweeperAlienEntity
 */

SweeperAlienEntity::SweeperAlienEntity(double x, double y, int evolution)
: Entity(32, x, y) {
	this->r = 255;
	this->g = 50;
	this->b = 50;
	this->a = 255;

	this->evolution = evolution;
	this->health += evolution;
}

void SweeperAlienEntity::onDamage(int damage) {
	health --;
	bump = 4;

	if (health <= 0) {
		this->dead = true;
	}
}

gls::Sprite SweeperAlienEntity::sprite(gls::TileSet& tileset) {
	return tileset.sprite(evolution, 5);
}

void SweeperAlienEntity::tick(Level& level) {
	this->x += facing * 2;
	this->y += bump;

	if (this->bump > 0) {
		this->bump -= 0.1;
	} else {
		this->bump = 0;
	}

	this->cooldown -= 0.05f;

	Collision collision = level.checkCollision(this);

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

		level.addEntity(new BulletEntity {-3, bx, y - 24, this});
	}
}

/*
 * TileEntity
 */

TileEntity::TileEntity(double x, double y, uint8_t tile, int tx, int ty)
: Entity(4, tx, ty) {
	float dx = x - tx;
	float dy = y - ty;

	this->tile = tile;
	this->fx = std::clamp(1 / (1 + dx), -8.0f, 8.0f);
	this->fy = std::clamp(1 / (1 + dy), -8.0f, 8.0f);
}

gls::Sprite TileEntity::sprite(gls::TileSet& tileset) {
	return getTileSprite(tileset, tile);
}

void TileEntity::tick(Level& level) {

	this->x += fx;
	this->y += fy;

	this->fx *= 0.9f;
	this->fy *= 0.9f;

//	if (!dead || level.checkCollision(this)) {
//		dead = true;
//	}

	float max_age = 10;
	this->a = 255 * (max_age - age) / max_age;

	if (age >= max_age) {
		dead = true;
	}

	age ++;
}