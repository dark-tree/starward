#include "entity.hpp"
#include "level.hpp"
#include "sounds.hpp"
#include "tile.hpp"
#include "emitter.hpp"

/*
 * Entity
 */

Entity::Entity(int radius, double size, double x, double y)
: tile_radius(radius), size(size) {
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

void Entity::move(Level& level, float x, float y) {
	this->x += x;
	this->y += y;
}

void Entity::clamp() {
	if (this->x < 0) {
		this->x = 0;
	}

	if (this->x > SW) {
		this->x = SW;
	}
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

void Entity::onDamage(Level& level, int damage, Entity* damager) {
	if (damage > 0) {
		this->dead = true;
	}
}

bool Entity::isCausedByPlayer() {
	return false;
}

Entity* Entity::getParent() {
	return nullptr;
}

gls::Sprite Entity::sprite(gls::TileSet& tileset) {
	return tileset.sprite(0, 0);
}

void Entity::draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) {
	emitSpriteQuad(writer, x, y + level.getScroll(), size, size, angle, sprite(tileset), r, g, b, a);
}

void Entity::tick(Level& level) {
	float wy = level.getScroll() + y;
	visible = (wy < SH);

	if (wy < -size) {
		dead = true;
	}

	if (dead) {
		visible = false;
	}

	age += 1;
}

/*
 * BulletEntity
 */

BulletEntity::BulletEntity(float velocity, double x, double y, Entity* parent)
: Entity(1, 8, x, y) {
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

bool BulletEntity::isCausedByPlayer() {
	return parent && parent->isCausedByPlayer();
}

Entity* BulletEntity::getParent() {
	return parent;
}

void BulletEntity::tick(Level& level) {
	y += velocity;

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
: Entity(1, 64, x, y) {}

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
: Entity(2, 64, SW / 2, 0) {
	this->r = 255;
	this->g = 255;
	this->b = 255;
	this->a = 255;
}

bool PlayerEntity::isCausedByPlayer() {
	return true;
}

bool PlayerEntity::shouldCollide(Entity* entity) {
	if (invulnerable > 0) {
		return false;
	}

	return Entity::shouldCollide(entity);
}

void PlayerEntity::onDamage(Level& level, int damage, Entity* damager) {
	if (damage > 0) {
		if (invulnerable <= 0) {
			if (lives <= 0) {
				level.setState(GameState::DEAD);
				SoundSystem::getInstance().add(Sounds::death).play();
				Entity::onDamage(level, damage, damager);
			}

			SoundSystem::getInstance().add(Sounds::getRandomBlow()).play();
			lives--;
			invulnerable = 300;
		}

		return;
	}

	if (damage == -10) {
		if (lives < 3) {
			lives ++;
		} else {
			level.addScore(1000);
		}
	}
}

gls::Sprite PlayerEntity::sprite(gls::TileSet& tileset) {
	if (invulnerable > 0) {
		this->a = 180;
	} else {
		this->a = 255;
	}

	if (invulnerable > 0) {
		return tileset.sprite(3 - (invulnerable & 0b1000 ? 1 : 0), 0);
	}

	return tileset.sprite(2, 0);
}

void PlayerEntity::draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) {
	Entity::draw(level, tileset, writer);

	for (int i = 0; i < lives; i ++) {
		emitSpriteQuad(writer, 32 + i * 48, SH - 32, 32, 32, 0, tileset.sprite(0, 1), 255, 255, 255, 220);
	}
}

void PlayerEntity::tick(Level& level) {

	this->y = 64 - level.getScroll() + level.getSkip() * 32;
	this->cooldown -= 0.1f;

	if (invulnerable == 0) {
		Collision collision = level.checkCollision(this);

		if (collision.type == Collision::TILE) {
			onDamage(level, 10, this);
		}
	}

	tilt *= 0.9;

	if (gls::Input::is_pressed(Key::LEFT)) {
		move(level, -6, 0);
		tilt -= 0.1;
	}

	if (gls::Input::is_pressed(Key::RIGHT)) {
		move(level, +6, 0);
		tilt += 0.1;
	}

	this->angle = tilt * 0.2;

	if ((cooldown <= 0) && gls::Input::is_pressed(Key::SPACE)) {
		level.addEntity(new BulletEntity {11, x, y + 64, this});
		SoundSystem::getInstance().add(Sounds::getRandomSoft()).play();
		cooldown = 1;
	}

	if (invulnerable > 0) {
		invulnerable --;
	}

	clamp();

}

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

void SweeperAlienEntity::onDamage(Level& level, int damage, Entity* damager) {
	if (damager && !damager->isCausedByPlayer()) {
		facing *= -1;
		return;
	}

	health --;
	bump = 4;

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
			level.addEntity(new BulletEntity{-3, bx, y - 24, this});
		}
	}

	Entity::tick(level);
}

/*
 * TileEntity
 */

TileEntity::TileEntity(double x, double y, uint8_t tile, int tx, int ty)
: Entity(0, 4, tx, ty) {
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

		if (tile == 3) {
			level.addScore(10);
		}
	}

	age ++;
}

/*
 * ExtraLiveEntity
 */

ExtraLiveEntity::ExtraLiveEntity(double x, double y)
: Entity(1, 32, x, y) {

}

void ExtraLiveEntity::onDamage(Level& level, int damage, Entity* damager) {
	Entity::onDamage(level, damage, damager);

	Entity* parent = damager->getParent();
	SoundSystem::getInstance().add(Sounds::coin).play();

	if (parent) {
		parent->onDamage(level, -10, this);
	}
}

gls::Sprite ExtraLiveEntity::sprite(gls::TileSet& tileset) {
	return tileset.sprite(0, 1);
}

void ExtraLiveEntity::tick(Level& level) {
	Entity::tick(level);
	this->angle = sin(this->age * 0.07f) * 0.5;
}