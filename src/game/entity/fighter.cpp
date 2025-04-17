
#include "fighter.hpp"
#include "bullet.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * FighterAlienEntity
 */

FighterAlienEntity::FighterAlienEntity(double x, double y, int evolution)
: Entity(2, 32, x, y) {

	// player offset targets
	this->px = -80;
	this->py = 220;

	this->evolution = evolution;
}

void FighterAlienEntity::forEachDanger(Level& level, const std::function<void(BulletEntity*, float, float)>& callback) {

	for (auto& entity : level.getEntities()) {
		if (BulletEntity* bullet = dynamic_cast<BulletEntity*>(entity.get())) {
			if (bullet->isCausedByPlayer()) {

				// from this to bullet
				float dx = bullet->x - this->x;
				float dy = bullet->y - this->y;

				// ignore bullets above us
				if (dy > 0) continue;

				// ignore bullets that are too far away
				if (dy < -200) continue;
				if (std::abs(dx) > 100) continue;

				callback(bullet, dx, dy);
			}
		}
	}

}

bool FighterAlienEntity::tickMovement(Level& level) {

	float abx = std::abs(vx);
	float aby = std::abs(vy);

	if (underhung && (abx < 1)) {
		abx = 1;
	}

	float ox = signum(vx) * std::min(abx, 2.0f);
	float oy = signum(vy) * std::min(aby, 2.0f);

	float px = this->x;
	float py = this->y;
	this->vx = ox * 0.8;
	this->vy = oy * 0.8;

	auto apply = [&] (float ax, float ay) {
		this->x += ax;
		this->y += ay;

		Collision c = level.checkCollision(this);

		if (c.type != Collision::MISS) {
			this->x = px;
			this->y = py;
			return false;
		}

		return true;
	};

	if (apply(ox, oy)) {
		down = false;
		underhung = false;
		return true;
	}

	if (abx < 0.3) {
		//underhung = true;
	}

	if (apply(ox, 0)) {
		this->vy *= -1;
		down = false;
		return true;
	}

	if (down) {
		if (apply(0, -oy)) {
			return true;
		}
	}

	if (apply(0, oy)) {
		this->vx *= -1;
		//underhung = false;
		return true;
	}

	down = true;
	return false;
}

bool FighterAlienEntity::checkPlacement(Level& level) {
	return level.checkCollision(this).type == Collision::MISS;
}

void FighterAlienEntity::onDamage(Level& level, int damage, Entity* damager) {
	if (damager && !damager->isCausedByPlayer()) {
		return;
	}

	if (damager && damager->isCausedByPlayer()) {
		this->dead = true;
		level.addScore(200);
	}
}

void FighterAlienEntity::tick(Level& level) {

	this->cooldown -= 0.05f;
	auto player = level.getPlayer();

	if (avoiding > 0) {
		avoiding --;
	}

	float ax = 0;
	float ay = 0;

	// make sure we only have one fighter at once
	for (auto& entity : level.getEntities()) {
		if (FighterAlienEntity* fighter = dynamic_cast<FighterAlienEntity*>(entity.get())) {
			if (!fighter->escape && (fighter != this)) {
				float dx = fighter->x - this->x;
				float dy = fighter->y - this->y;

				float len = sqrt(dx * dx + dy * dy);

				if (len < 100) {
					this->escape = true;
					break;
				}
			}
		}
	}

	forEachDanger(level, [&] (BulletEntity* bullet, float dx, float dy) {
		// otherwise try to avoid
		ax -= signum(dx) * 2;

		// close call!
		if (std::abs(dx) < 30) {
			ay += 3;
		}

		avoiding = 30;
	});

	// add avoidance vector
	this->vx += ax;
	this->vy += ay;

	if (player) {

		// vector from this to player
		float dx = player->x - this->x;
		float dy = player->y - this->y;

		if (avoiding > 0) {
			this->px = -dx + ax * 5;
			//this->py = -dy + ay * 5;
		}

		float tx = px + dx;

		if ((std::abs(tx) < 4) && (avoiding <= 0)) {
			this->px *= -1;

//			if (px > 0) {
//				this->px = -100;
//			} else {
//				this->px = +100;
//			}
		}

		if (!escape) {
			this->vx = std::lerp(dx + px, vx, 0.99f);
			this->vy = std::lerp(dy + py, vy, 0.99f);
		}

	} else {
		escape = true;
	}

	if (escape) {

		// move off-screen
		int half = SW / 2;

		if (x <= half) {
			this->vx -= 1;
		} else {
			this->vx += 1;
		}

		this->vy = sin(age * 0.02);

	}

	if ((cooldown <= 0) && (avoiding <= 0)) {
		cooldown = 1;

		if (visible) {
			level.addEntity(new BulletEntity{-3, x, y - 24, self()});
		}
	}

	tickMovement(level);
	Entity::tick(level);
}

void FighterAlienEntity::draw(Level& level, gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& writer) {
	emitEntityQuad(level, writer, tileset.sprite(evolution, 7), size, angle, Color::red());

	auto player = level.getPlayer();

	if (player) {
		float tx = px + player->x;
		float ty = py + player->y + level.getScroll();

		int ox = 0;

		emitSpriteQuad(writer, tx, ty - 16, 16, 16, angle, tileset.sprite(6, 1), 0, 255, 0, 255);
		emitLineQuad(writer, x, y + level.getScroll(), tx, ty - 16, 2, tileset.sprite(0, 0), 0, 255, 0, 100);

		if (down) {
			ox += 24;
			emitSpriteQuad(writer, tx + ox, ty - 16, 16, 16, angle, tileset.sprite(0, 0), 155, 155, 0, 255);
		}

		if (underhung) {
			ox += 24;
			emitSpriteQuad(writer, tx + ox, ty - 16, 16, 16, angle, tileset.sprite(0, 0), 255, 0, 0, 255);
		}

		if (escape) {
			ox += 24;
			emitSpriteQuad(writer, tx + ox, ty - 16, 16, 16, angle, tileset.sprite(0, 0), 0, 0, 255, 255);
		}
	}

	forEachDanger(level, [&] (BulletEntity* bullet, float dx, float dy) {
		emitLineQuad(writer, x, y + level.getScroll(), bullet->x, bullet->y + level.getScroll() - 16, 2, tileset.sprite(0, 0), 255, 0, 0, 100);
	});

}