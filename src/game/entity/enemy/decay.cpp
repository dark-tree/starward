#include "decay.hpp"

#include "game/sounds.hpp"
#include "game/entity/bullet.hpp"
#include "game/level/level.hpp"
#include "sound/system.hpp"

DecaySharedState::~DecaySharedState() {
	printf("Unloaded shared decay state\n");
}

std::shared_ptr<DecayEntity> DecaySharedState::createPart(float x, float y) {
	auto self = shared_from_this();
	auto part = std::make_shared<DecayEntity>(x, y, self);

	glm::ivec2 vec {x/32, y/32};
	parts[vec] = part;

	return part;
}

std::shared_ptr<DecayEntity> DecaySharedState::getPart(float x, float y) {
	glm::ivec2 vec {x/32, y/32};
	return parts[vec].lock();
}

DecayEntity::DecayEntity(float x, float y, const std::shared_ptr<DecaySharedState>& shared)
: AlienEntity(x, y, 0), shared(shared) {
	this->size = 24;
	base = randomInt(0, 7);
	row_1 = randomInt(0, 7);
	row_2 = randomInt(0, 7);
}

bool DecayEntity::checkPlacement(Level& level) {
	return true;
}

// void DecayEntity::onDamage(Level& level, int damage, Entity* damager) {
// 	Entity::onDamage(level, damage, damager);
// }

void DecayEntity::tick(Level& level) {
	AlienEntity::tick(level);

	if (timer == 0) {
		timer = randomInt(60, 400);

		base = randomInt(0, 7);
		row_1 = randomInt(0, 7);
		row_2 = randomInt(0, 7);

		if (shared->getPart(x, y - 32) == nullptr) {
			level.addEntity(new BulletEntity{-3, x, y - 24, self()});
		}

		cb = Color::of(200, randomInt(50, 100), randomInt(50, 100));
	} else {
		timer --;
	}

	Collision collision = level.checkCollision(this);

	if (collision.type == Collision::ENTITY) {
		if (collision.entity) {
			collision.entity->onDamage(level, 1, this);
		}
	}

	if (visible) {
		level.addSlowness(0.99);
	}
}

void DecayEntity::draw(Level& level, Renderer& renderer) {
	auto& writer = *renderer.terrain.writer;
	auto& tileset = *renderer.terrain.tileset;

	emitEntityQuad(level, writer, tileset.sprite(base, 10), size, angle, cb);
	emitEntityQuad(level, writer, tileset.sprite(row_1, 12), size, angle, Color::red(true));
	emitEntityQuad(level, writer, tileset.sprite(row_2, 11), size, angle, Color::red(true));
}
