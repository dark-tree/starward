#include "entity.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * Entity
 */

void Entity::emitEntityQuad(Level& level, BufferWriter<Vert4f4b>& writer, Sprite sprite, float size, float angle, Color color) const {
	emitSpriteQuad(writer, x, y + level.getScroll(), size, size, angle, sprite, color.r, color.g, color.b, color.a);
}

void Entity::emitBoxWireframe(Box box, BufferWriter<Vert4f4b>& writer, Sprite sprite, float width, Color color) const {
	emitLineQuad(writer, box.x, box.y, box.x, box.y + box.h, width, sprite, color.r, color.g, color.b, color.a);
	emitLineQuad(writer, box.x, box.y, box.x + box.w, box.y, width, sprite, color.r, color.g, color.b, color.a);
	emitLineQuad(writer, box.x + box.w, box.y, box.x + box.w, box.y + box.h, width, sprite, color.r, color.g, color.b, color.a);
	emitLineQuad(writer, box.x, box.y + box.h, box.x + box.w, box.y + box.h, width, sprite, color.r, color.g, color.b, color.a);
}

Entity::Entity(float size, float x, float y)
: size(size) {
	this->x = x;
	this->y = y;
	this->collider = Box {-size/2, -size/2, size, size};
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

void Entity::debugDraw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) {
	emitBoxWireframe(getBoxCollider().withOffset(0, level.getScroll()), writer, tileset.sprite(0, 0), 1, Color::white());
}

bool Entity::checkPlacement(Level& level) {
	return false;
}

bool Entity::shouldCollide(Entity* entity) {
	return getBoxCollider().intersects(entity->getBoxCollider());
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

Box Entity::getBoxCollider() const {
	return collider.withOffset(x, y);
}
