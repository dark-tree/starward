
#include "player.hpp"
#include "bullet.hpp"
#include "powerup.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * PlayerEntity
 */

PlayerEntity::PlayerEntity()
: Entity(2, 64, SW / 2, 0) {

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

			SoundSystem::getInstance().add(Sounds::blow).play();
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

		SoundSystem::getInstance().add(shot ? Sounds::soft : Sounds::empty).play();
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

void PlayerEntity::draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) {

	gls::Sprite sprite = tileset.sprite(2, 0);

	if (invulnerable > 0) {
		sprite = tileset.sprite(3 - (invulnerable & 0b1000 ? 1 : 0), 0);
	}

	emitEntityQuad(level, writer, sprite, size, angle, Color::white().withAlpha(invulnerable > 0 ? 180 : 255));

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