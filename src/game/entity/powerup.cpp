
#include "powerup.hpp"

#include "player.hpp"
#include "particle/blow.hpp"
#include "game/level/level.hpp"
#include "game/sounds.hpp"
#include "particle/text.hpp"

/*
 * PowerUpEntity
 */

PowerUpEntity::Type PowerUpEntity::randomPick() {
	int pick = randomInt(0, 7);

	if (pick == 0) return LIVE;
	if (pick == 1) return DOUBLE_BARREL;
	if (pick == 2) return SHIELD;
	if (pick == 3) return NITRO;
	if (pick == 4) return STAN;
	if (pick == 5) return PIERCE;
	if (pick == 6) return BORING;
	if (pick == 7) return GUIDED;

	printf("Invalid power up picked!\n");
	return randomPick();
}

PowerUpEntity::PowerUpEntity(double x, double y, Type type)
: Entity(32, x, y), type(type) {
	this->collider = Box {-32, -32, 64, 64};
}

bool PowerUpEntity::checkPlacement(Level& level) {
	return true;
}

void PowerUpEntity::applyEffect(Level& level, PlayerEntity* player) {
	if (type == LIVE) {
		player->onDamage(level, -10, this);
		level.addEntity(new TextEntity {x, y, "Extra Life", 30});
	}

	if (type == DOUBLE_BARREL) {
		player->double_barrel_ticks += 50;
		level.addEntity(new TextEntity {x, y, "Double Barrel", 30});
	}

	if (type == SHIELD) {
		player->enableShield(level);
		level.addEntity(new TextEntity {x, y, "Shield", 30});
	}

	if (type == NITRO) {
		player->nitro_ticks += 8 * 60;
		level.addEntity(new TextEntity {x, y, "Nitro Charge", 30});
	}

	if (type == STAN) {
		player->charged_ammo += 30;
		level.addEntity(new TextEntity {x, y, "Charged Bullets", 30});
	}

	if (type == PIERCE) {
		player->piercing_ammo += 30;
		level.addEntity(new TextEntity {x, y, "Piercing Bullets", 30});
	}

	if (type == BORING) {
		player->boring_ammo += 30;
		level.addEntity(new TextEntity {x, y, "Drilling Bullets", 30});
	}

	if (type == GUIDED) {
		player->guided_ammo += 20;
		level.addEntity(new TextEntity {x, y, "Guided Bullets", 30});
	}
}

void PowerUpEntity::onDamage(Level& level, int damage, Entity* damager) {
	if (damager->isCausedByPlayer() && visible) {
		std::shared_ptr<Entity> parent = damager->getParent();
		SoundSystem::getInstance().add(Sounds::coin).play();

		if (!dead && parent) {
			applyEffect(level, dynamic_cast<PlayerEntity*>(parent.get()));
			dead = true;
		}

		Entity::onDamage(level, damage, damager);
	}
}

bool PowerUpEntity::shouldAutoTarget() {
	return true;
}

void PowerUpEntity::tick(Level& level) {
	this->angle = sin(this->age * 0.07f) * 0.5;

	Collision collision = level.checkCollision(this);

	if (!dead && (collision.type == Collision::ENTITY)) {
		if (PlayerEntity* player = dynamic_cast<PlayerEntity*>(collision.entity)) {
			level.addEntity(new BlowEntity(x, y));
			SoundSystem::getInstance().add(Sounds::coin).play();

			applyEffect(level, player);
			dead = true;
		}
	}

	Entity::tick(level);
}

void PowerUpEntity::draw(Level& level, Renderer& renderer) {
	emitEntityQuad(level, *renderer.terrain.writer, renderer.terrain.tileset->sprite(7, 4), size + 4 * sin(age * 0.1), 0, Color::white().withAlpha(180));
	emitEntityQuad(level, *renderer.terrain.writer, renderer.terrain.tileset->sprite(type, 1), size , angle, Color::white());
}
