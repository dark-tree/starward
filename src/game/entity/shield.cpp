
#include "shield.hpp"

#include "bullet.hpp"
#include "player.hpp"
#include "game/emitter.hpp"
#include "game/level/level.hpp"

/*
 * ShieldEntity
 */

ShieldEntity::ShieldEntity(const std::shared_ptr <PlayerEntity>& player)
: player(player), Entity(32, 0, 0) {
	updatePosition();
	repower();
	this->collider = Box {-32, -16, 64, 32};
}

bool ShieldEntity::shouldCollide(Entity* entity) {
	if (entity->isCausedByPlayer()) {
		return false;
	}

	return Entity::shouldCollide(entity);
}

void ShieldEntity::updatePosition() {
	this->x = player->x;
	this->y = player->y + 48 + (age % 60 / 20) * 2;
}

void ShieldEntity::onDamage(Level& level, int damage, Entity* damager) {
	if (damager && damager->isCausedByPlayer()) {
		return;
	}

	if (damage > 0) {
		damaged = true;
	}
}

void ShieldEntity::tick(Level& level) {
	Entity::tick(level);
	updatePosition();

	if (player->isDead()) {
		dead = true;
	}

	if (damaged) {
		power --;

		if (power == 0) {
			dead = true;
		}
	}
}

void ShieldEntity::draw(Level& level, Renderer& renderer) {
	auto& writer = *renderer.terrain.writer;
	auto& tileset = *renderer.terrain.tileset;

	Color c = Color::white().withAlpha(power / 60.0f * 200);

	int offset = age % 40 / 10;
	emitSpriteQuad(writer, x + player->getAngle() * 40, y + collider.y + level.getScroll(), 64, 32, player->getAngle(), tileset.sprite(4 + offset, 0), c.r, c.g, c.b, c.a);
}

void ShieldEntity::repower() {
	damaged = false;
	power = 60;
}
