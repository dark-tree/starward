
#include "powerup.hpp"
#include "player.hpp"
#include "blow.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

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
: Entity(32, x, y), type(type) {

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

void PowerUpEntity::draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) {
	emitEntityQuad(level, writer, tileset.sprite((int) type, 1), size, angle, Color::white());
}
