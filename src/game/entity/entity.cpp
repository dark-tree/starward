#include "entity.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

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
