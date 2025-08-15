
#include "player.hpp"

#include "bullet.hpp"
#include "particle/dust.hpp"
#include "powerup.hpp"
#include "shield.hpp"
#include "game/level/level.hpp"
#include "game/level/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * PlayerEntity
 */

Box PlayerEntity::getBoxBumper(int side) const {
	return bumper.withOffset(side * 32, 0).withOffset(x, y);
}

void PlayerEntity::onUserInput(Level& level) {
	if (first_input) {
		first_input = false;
		level.loadPlayCount();
	}
}

PlayerEntity::PlayerEntity()
: Entity(64, SW / 2, 0) {
	this->bumper = Box(-5, -32, 10, 64);
	this->collider = Box {-24, -24, 48, 48};
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
		if (level.isDebug()) {
			return;
		}

		if (invulnerable <= 0) {
			if (lives <= 0) {
				level.setState(GameState::DEAD);
				SoundSystem::getInstance().add(Sounds::death).play();
				Entity::onDamage(level, damage, damager);

				for (int i = randomInt(50, 80); i > 0; i--) {
					int brightness = randomInt(50, 100);
					Color color = Color::of(brightness, brightness, 255);
					level.addEntity(new DustEntity {x, y, randomFloat(-1, 1), randomFloat(-1, 1), 1, 1, 1, 60, color});
				}
			}

			SoundSystem::getInstance().add(Sounds::blow).play();
			lives--;
			invulnerable = 200;
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

void PlayerEntity::tick(Level& level) {

	if (shield && shield->shouldRemove()) {
		shield.reset();
	}

	int avoidance = 0;

	// regenerate ammo
	if (age % 20 == 0) {
		if (ammo < 64) ammo ++;
	}

	this->y = 64 - level.getScroll() + level.getSkip() * 40;
	this->cooldown -= 0.1f;

	// spawn engine plum particles
	if (level.getSkip() > 0.5) {
		float spread = randomFloat(-10, 10);
		int brightness = randomInt(50, 100);
		Color color = Color::of(brightness, brightness, 255);
		level.addEntity(new DustEntity(x + spread, y - 42, 0, -1, 0, 1, 2, 20, color));
	}

	if (invulnerable == 0) {
		Collision collision = level.checkTileCollision(getBoxCollider());

		if (collision.type == Collision::TILE) {
			onDamage(level, 10, this);
		} else {
			Collision left = level.checkTileCollision(getBoxBumper(-1));
			Collision right = level.checkTileCollision(getBoxBumper(+1));

			if (left.type == Collision::TILE) avoidance += 1;
			if (right.type == Collision::TILE) avoidance -= 1;
		}
	}

	tilt *= 0.9;
	this->x += avoidance * 0.4;

	if ((avoidance <= 0) && Input::isPressed(Key::LEFT) || Input::isPressed(Key::A)) {
		this->x -= 6;
		tilt -= 0.1;
		onUserInput(level);
	}

	if ((avoidance >= 0) && Input::isPressed(Key::RIGHT) || Input::isPressed(Key::D)) {
		this->x += 6;
		tilt += 0.1;
		onUserInput(level);
	}

	this->angle = tilt * 0.2;

	if ((cooldown <= 0) && Input::isPressed(Key::SPACE)) {
		bool shot = false;
		cooldown = 1;

		if (double_barrel_ticks > 0) {
			level.addEntity(new BulletEntity {11, x - 32, y + 30, self()});
			level.addEntity(new BulletEntity {11, x + 32, y + 30, self()});
			double_barrel_ticks --;
			shot = true;
		} else {
			if (ammo > 0) {
				ammo --;
				level.addEntity(new BulletEntity {11, x, y + 48, self()});
				shot = true;
			}
		}

		SoundSystem::getInstance().add(shot ? Sounds::soft : Sounds::empty).play().volume(shot ? 0.8 : 1);
	}

	if (invulnerable > 0) {
		invulnerable --;
	}

	clamp();
	Entity::tick(level);
}

void PlayerEntity::draw(Level& level, Renderer& renderer) {

	auto& layer = renderer.terrain;
	auto& writer = *layer.writer;
	auto& tileset = *layer.tileset;

	Sprite sprite = tileset.sprite(2, 0);

	if (invulnerable > 0) {
		sprite = tileset.sprite(3 - (invulnerable & 0b1000 ? 1 : 0), 0);
	}

	const float vert = size + level.getSkip() * 8;
	Color c = Color::white().withAlpha(invulnerable > 0 ? 180 : 255);
	emitSpriteQuad(writer, x, y + level.getScroll(), size, vert, angle, sprite, c.r, c.g, c.b, c.a);

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

void PlayerEntity::debugDraw(Level& level, Renderer& renderer) {
	Entity::debugDraw(level, renderer);

	auto& layer = renderer.terrain;
	emitBoxWireframe(getBoxBumper(-1).withOffset(0, level.getScroll()), layer, 1, Color::white());
	emitBoxWireframe(getBoxBumper(+1).withOffset(0, level.getScroll()), layer, 1, Color::white());
}

void PlayerEntity::enableShield(Level& level) {
	if (shield) {
		shield->repower();
	} else {
		shield = level.addEntity(new ShieldEntity(std::static_pointer_cast<PlayerEntity>(self())));
	}
}
