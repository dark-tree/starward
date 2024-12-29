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

bool Entity::checkPlacement(Level& level) {
	return false;
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

std::shared_ptr<Entity> Entity::getParent() {
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

std::shared_ptr<Entity> Entity::self() {
	return shared_from_this();
}

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
	if ((invulnerable > 0) && !dynamic_cast<PowerUpEntity*>(entity)) {
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

	int pack = 8;
	int magazines = ammo / pack;
	int modulo = ammo % pack;
	int unit = 255 / pack * modulo;

	for (int i = 0; i < lives; i ++) {
		emitSpriteQuad(writer, 32 + i * 48, SH - 32, 32, 32, 0, tileset.sprite(0, 1), 255, 255, 255, 220);
	}

	for (int i = 0; i < magazines; i ++) {
		emitSpriteQuad(writer, 16 + i * 16, 16, 6, 6, 0, tileset.sprite(0, 0), 155, 155, 255, 220);
	}

	if (modulo) {
		emitSpriteQuad(writer, 16 + magazines * 16, 16, 6, 6, 0, tileset.sprite(0, 0), 155, 155, 255, unit);
	}
}

void PlayerEntity::tick(Level& level) {

	if (age % 20 == 0) {
		if (ammo < 64) ammo ++;
	}

	this->y = 64 - level.getScroll() + level.getSkip() * 32;
	this->cooldown -= 0.1f;

	if (invulnerable == 0) {
		Collision collision = level.checkCollision(this);

		if (collision.type == Collision::TILE) {
			onDamage(level, 10, this);
		}
	}

	tilt *= 0.9;

	if (gls::Input::is_pressed(Key::LEFT) || gls::Input::is_pressed(Key::A)) {
		move(level, -6, 0);
		tilt -= 0.1;
	}

	if (gls::Input::is_pressed(Key::RIGHT) || gls::Input::is_pressed(Key::D)) {
		move(level, +6, 0);
		tilt += 0.1;
	}

	this->angle = tilt * 0.2;

	if ((cooldown <= 0) && gls::Input::is_pressed(Key::SPACE)) {
		bool shot = false;
		cooldown = 1;

		if (double_barrel_ticks > 0) {
			level.addEntity(new BulletEntity {11, x - 32, y + 30, self()});
			level.addEntity(new BulletEntity {11, x + 32, y + 30, self()});
			shot = true;
		} else {
			if (ammo > 0) {
				ammo --;
				level.addEntity(new BulletEntity {11, x, y + 48, self()});
				shot = true;
			}
		}

		SoundSystem::getInstance().add(shot ? Sounds::getRandomSoft() : Sounds::getRandomEmpty()).play();
	}

	if (invulnerable > 0) {
		invulnerable --;
	}

	if (double_barrel_ticks > 0) {
		double_barrel_ticks --;
	}

	clamp();
	Entity::tick(level);
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
			SoundSystem::getInstance().add(Sounds::getRandomBlow()).play();

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
 * PowerUpEntity
 */

PowerUpEntity::Type PowerUpEntity::randomPick() {
	int pick = randomInt(0, 100);
	if (pick >=   0 && pick <=  40) return LIVE;
	if (pick >=  40 && pick <= 100) return DOUBLE_BARREL;

	return randomPick();
}

PowerUpEntity::PowerUpEntity(double x, double y, Type type)
: Entity(1, 32, x, y), type(type) {

}

bool PowerUpEntity::checkPlacement(Level& level) {
	return true;
}

void PowerUpEntity::applyEffect(Level& level, PlayerEntity* player) {
	if (type == Type::LIVE) {
		player->onDamage(level, -10, this);
	}

	if (type == Type::DOUBLE_BARREL) {
		player->double_barrel_ticks = 600;
	}
}

void PowerUpEntity::onDamage(Level& level, int damage, Entity* damager) {
	if (damager->isCausedByPlayer()) {
		std::shared_ptr<Entity> parent = damager->getParent();
		SoundSystem::getInstance().add(Sounds::coin).play();

		if (!dead && parent) {
			applyEffect(level, dynamic_cast<PlayerEntity*>(parent.get()));
			dead = true;
		}

		Entity::onDamage(level, damage, damager);
	}
}

gls::Sprite PowerUpEntity::sprite(gls::TileSet& tileset) {
	return tileset.sprite((int) type, 1);
}

void PowerUpEntity::tick(Level& level) {
	this->angle = sin(this->age * 0.07f) * 0.5;

	Collision collision = level.checkCollision(this);

	if (!dead && (collision.type == Collision::ENTITY)) {
		if(PlayerEntity* player = dynamic_cast<PlayerEntity*>(collision.entity)) {
			level.addEntity(new BlowEntity(x, y));
			SoundSystem::getInstance().add(Sounds::coin).play();

			applyEffect(level, player);
			dead = true;
		}
	}

	Entity::tick(level);
}

/*
 * TurretAlienEntity
 */

TurretAlienEntity::TurretAlienEntity(double x, double y, int evolution)
: Entity(2, 32, x, y) {
	this->r = 255;
	this->g = 50;
	this->b = 50;
	this->a = 255;

	this->evolution = evolution;
}

bool TurretAlienEntity::checkPlacement(Level& level) {
	Collision collision = level.checkCollision(this);

	if (collision.type == Collision::ENTITY) {
		return false;
	}

	return true;
}

void TurretAlienEntity::onDamage(Level& level, int damage, Entity* damager) {
	if (damager && damager->isCausedByPlayer()) {
		level.addScore(100);
		this->dead = true;
	}
}

gls::Sprite TurretAlienEntity::sprite(gls::TileSet& tileset) {
	return tileset.sprite(0, 6);
}

void TurretAlienEntity::shoot(Level& level, float speed, float radius, float offset) {

	// calculate shooting position
	float angle = deg(90) - head + offset;
	float dx = radius * cos(angle);
	float dy = radius * sin(angle);

	// create bullet
	level.addEntity(new BulletEntity {-speed, x + dx, y + dy, self(), head});

}

void TurretAlienEntity::tick(Level& level) {
	Entity::tick(level);

	if (std::shared_ptr<PlayerEntity> player = level.getPlayer()) {
		glm::vec2 dir {player->x - x, player->y - y};
		float bullet = 5;
		float radius = 32;

		// try to be sneaky and target future position
		float oy = level.getSpeed() * (glm::length(dir) - radius) / bullet;
		target = deg(90) - std::atan2(dir.y + oy, dir.x);
		head = slerp(head, target, deg(1 + evolution));

		if (cooldown <= 0) {
			cooldown = 4.05f - evolution;

			if (evolution >= 2) {
				barrel = -barrel;
				cooldown /= 2;

				if (barrel == 1) {
					shoot(level, bullet, radius, -deg(20));
				} else {
					shoot(level, bullet, radius, +deg(20));
				}
			} else {
				shoot(level, bullet, radius, 0);
			}
		}

		this->cooldown -= 0.05f;
	}
}

void TurretAlienEntity::draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) {
	this->a = 200;
	Entity::draw(level, tileset, writer);

	emitSpriteQuad(writer, x, y + level.getScroll(), size, size, head, tileset.sprite(evolution + 1, 6), r, g, b, 255);
}