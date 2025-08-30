
#include "entity.hpp"

#include "game/level/level.hpp"
#include "game/level/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * Entity
 */

void Entity::emitEntityQuad(Level& level, BufferWriter<VertLevel>& writer, Sprite sprite, float size, float angle, Color color) const {
	emitSpriteQuad(writer, x, y + level.getScroll(), size, size, angle, sprite, color.r, color.g, color.b, color.a);
}

void Entity::emitBoxWireframe(Box box, RenderLayer& layer, float width, Color color) const {
	Sprite sprite = layer.tileset->sprite(0, 0);

	emitLineQuad(*layer.writer, box.x, box.y, box.x, box.y + box.h, width, sprite, color.r, color.g, color.b, color.a);
	emitLineQuad(*layer.writer, box.x, box.y, box.x + box.w, box.y, width, sprite, color.r, color.g, color.b, color.a);
	emitLineQuad(*layer.writer, box.x + box.w, box.y, box.x + box.w, box.y + box.h, width, sprite, color.r, color.g, color.b, color.a);
	emitLineQuad(*layer.writer, box.x, box.y + box.h, box.x + box.w, box.y + box.h, width, sprite, color.r, color.g, color.b, color.a);
}

float Entity::getAngle() const {
	return angle;
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

void Entity::clamp() {
	if (this->x < 0) {
		this->x = 0;
	}

	if (this->x > SW) {
		this->x = SW;
	}
}

void Entity::debugDraw(Level& level, Renderer& renderer) {
	emitBoxWireframe(getBoxCollider().withOffset(0, level.getScroll()), renderer.terrain, 1, Color::white());
}

void Entity::onSpawned(const Level& level, NULLABLE Segment* segment) {
	// by default do nothing, Turrets can Tesla towers use it to generate foundations
}

void Entity::onDespawn(Level& level) {
	// by default do nothing when we despawn
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
		onDespawn(level);
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

bool Entity::isDead() const {
	return dead;
}
