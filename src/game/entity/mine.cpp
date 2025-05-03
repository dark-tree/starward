#include "mine.hpp"

#include <game/level.hpp>
#include <game/sounds.hpp>
#include <sound/system.hpp>

#include "blow.hpp"
#include "bullet.hpp"
#include "player.hpp"

MineAlienEntity::MineAlienEntity(float x, float y)
: AlienEntity(x, y, 0) {
	timer = randomInt(0, 60);
}

bool MineAlienEntity::checkPlacement(Level& level) {
	return level.checkCollision(this).type == Collision::MISS;
}

void MineAlienEntity::onKilled(Level& level) {
	tickExplode(level);
}

void MineAlienEntity::tickExplode(Level& level) {
	spawnParticles(level, 5, 10);
	level.addScore(150);
	this->dead = true;

	int bullets = 10;
	float step = 2 * M_PI / bullets;

	for (int i = 0; i < bullets; i++) {
		float angle = i * step;
		float bx = x + 24 * cos(angle);
		float by = y + 24 * sin(angle);

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

	if (distance < 150) {
		level.addEntity(new BlowEntity(x, y));
		SoundSystem::getInstance().add(Sounds::damage).play();
		tickExplode(level);
	}
}

void MineAlienEntity::draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) {
	emitEntityQuad(level, writer, tileset.sprite(led, 9), size, angle, Color::red(damage_ticks));
}
