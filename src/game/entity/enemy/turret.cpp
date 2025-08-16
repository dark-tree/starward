
#include "turret.hpp"

#include "game/entity/bullet.hpp"
#include "game/entity/particle/dust.hpp"
#include "game/level/level.hpp"
#include "game/level/matcher.hpp"

/*
 * TurretAlienEntity
 */

bool TurretAlienEntity::spawn(Level& level, Segment& segment, int evolution) {

	int cols[Segment::width];
	randomBuffer(cols, Segment::width);

	for (int col = 0; col < Segment::width; col++) {

		int rows[Segment::height];
		randomBuffer(rows, Segment::height);

		for (int row = 0; row < Segment::height; row++) {

			TerrainMacher matcher {segment, cols[col], rows[row]};

			if (!matcher.acceptDown(true, 4, 10)) {
				continue;
			}

			glm::ivec2 tile = matcher.here();

			if (!matcher.acceptDown(false, 5, 200)) {
				continue;
			}

			glm::vec2 pos = Level::toEntityPos(tile.x, tile.y + segment.getStartY());
			return level.trySpawn(new TurretAlienEntity {pos.x, pos.y, evolution});
		}
	}

	printf("Failed to find turret placement spot!\n");
	return false;
}

TurretAlienEntity::TurretAlienEntity(double x, double y, int evolution)
	: AlienEntity(x, y, evolution) {
}

bool TurretAlienEntity::checkPlacement(Level& level) {
	return level.checkEntityCollision(this).type == Collision::MISS;
}

void TurretAlienEntity::shoot(Level& level, float speed, float radius, float offset) {

	// calculate shooting position
	float angle = deg(90) - head + offset;
	float ax = cos(angle);
	float ay = sin(angle);

	float effect = radius - 8;

	// create bullet
	level.addEntity(new BulletEntity {-speed, x + radius * ax, y + radius * ay, self(), head});

	// particle effect
	for (int i = randomInt(2, 5); i > 0; i--) {
		level.addEntity(new DustEntity {x + effect * ax, y + effect * ay, ax, ay, head, 1, 0.5, 20, Color::red()});
	}

}

void TurretAlienEntity::tick(Level& level) {
	AlienEntity::tick(level);

	if (std::shared_ptr<PlayerEntity> player = level.getPlayer()) {
		glm::vec2 dir {player->x - x, player->y - y};
		float bullet = 5;
		float radius = 32;

		// try to be sneaky and target future player position
		float oy = level.getSpeed() * (glm::length(dir) - radius) / bullet;
		target = deg(90) - std::atan2(dir.y + oy, dir.x);
		head = slerp(head, target, deg(1 + evolution));

		if (cooldown <= 0) {
			cooldown = 4.05f - evolution;

			if (evolution >= 2) {
				barrel = -barrel;
				cooldown /= 2;

				if (barrel == 1) {
					shoot(level, bullet, radius, -deg(20));
				} else {
					shoot(level, bullet, radius, +deg(20));
				}
			} else {
				shoot(level, bullet, radius, 0);
			}
		}

		this->cooldown -= 0.05f;
	}
}

void TurretAlienEntity::draw(Level& level, Renderer& renderer) {
	const Color color = Color::red(damage_ticks);

	emitEntityQuad(level, *renderer.terrain.writer, renderer.terrain.tileset->sprite(0, 6), size, angle, color);
	emitEntityQuad(level, *renderer.terrain.writer, renderer.terrain.tileset->sprite(evolution + 1, 6), size, head, color);
}

void TurretAlienEntity::onSpawned(const Level& level, Segment* segment) {

	if (segment == nullptr) {
		return;
	}

	int oy = 0;
	glm::ivec2 pos = level.toTilePos(x, y);

	const int count = 6;
	int heights[count]  = {2, 3, 3, 2, 2, 3};
	int material[count] = {0, 0, 0, 2, 2, 2};

	for (int i = 0; i < count; i ++) {

		int h = heights[i];
		int m = material[i];

		for (int x = -h; x <= h; x ++) {
			int tx = pos.x + x;
			int ty = pos.y + oy - segment->getStartY();

			if (tx < 0 || tx >= Segment::width || ty < 0 || ty >= Segment::height) {
				continue;
			}

			segment->at(tx, ty) = m;
		}

		oy ++;
	}
}
