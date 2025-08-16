
#include "mine.hpp"

#include <game/level/level.hpp>
#include <game/sounds.hpp>
#include <sound/system.hpp>
#include "game/entity/particle/blow.hpp"
#include "game/entity/bullet.hpp"
#include "game/entity/player.hpp"

MineAlienEntity::MineAlienEntity(float x, float y, int evolution)
: AlienEntity(x, y, evolution) {
	timer = randomInt(0, 60);

	if (evolution) {
		this->collider = Box {-20, -20, 40, 40};
	}
}

bool MineAlienEntity::checkPlacement(Level& level) {
	return level.checkCollision(this).type == Collision::MISS;
}

void MineAlienEntity::onDamage(Level& level, int damage, Entity* damager) {
	bool reduced = false;

	if (BulletEntity* bullet = dynamic_cast<BulletEntity*>(damager)) {
		if (bullet->isCharged()) reduced = true;
	}

	tickExplode(level, reduced);
}

void MineAlienEntity::onDespawn(Level& level) {
	level.addScore(evolution * -10);
}

void MineAlienEntity::tickExplode(Level& level, bool reduced) {
	spawnParticles(level, 5, 10);
	level.addScore(100);
	this->dead = true;

	float start = randomFloat(-M_PI, M_PI);
	int bullets = (evolution ? 16 : 10) - (reduced ? 5 : 0);
	float step = 2 * M_PI / bullets;
	int radius = evolution ? 32 : 24;

	for (int i = 0; i < bullets; i++) {
		float angle = i * step + start;
		float bx = x + radius * cos(angle);
		float by = y + radius * sin(angle);

		level.addEntity(new BulletEntity{-3, bx, by, self(), (float) M_PI_2 - angle});
	}
}

void MineAlienEntity::tick(Level& level) {
	AlienEntity::tick(level);

	this->distance = std::numeric_limits<float>::max();

	for (auto& entity : level.getEntities()) {
		if (PlayerEntity* player = dynamic_cast<PlayerEntity*>(entity.get())) {

			glm::vec2 target {player->x, player->y};
			glm::vec2 self {x, y};

			this->distance = glm::distance(target, self);
			break;
		}
	}

	if (timer) {
		timer --;
	} else {
		timer = 5 + distance / 20;
		led = !led;
	}

	if (distance < (evolution ? 200 : 150)) {
		level.addEntity(new BlowEntity(x, y));
		SoundSystem::getInstance().add(Sounds::damage).play();
		tickExplode(level, false);
	}
}

void MineAlienEntity::draw(Level& level, Renderer& renderer) {
	const int offset = evolution ? 2 : 0;
	emitEntityQuad(level, *renderer.terrain.writer, renderer.terrain.tileset->sprite(led + offset, 9), size, angle, Color::red(damage_ticks));
}
