#include "entity.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * Entity
 */

void Entity::emitEntityQuad(Level& level, gls::BufferWriter<gls::Vert4f4b>& writer, gls::Sprite sprite, float size, float angle, Color color) const {
	emitSpriteQuad(writer, x, y + level.getScroll(), size, size, angle, sprite, color.r, color.g, color.b, color.a);
}

Entity::Entity(int radius, double size, double x, double y)
: tile_radius(radius), size(size) {
	this->x = x;
	this->y = y;
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
