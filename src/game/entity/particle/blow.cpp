
#include "blow.hpp"

#include "game/level/level.hpp"
#include "../../level/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * BlowEntity
 */

BlowEntity::BlowEntity(double x, double y)
: Entity(64, x, y) {}

bool BlowEntity::shouldCollide(Entity* entity) {
	return false;
}

void BlowEntity::tick(Level& level) {
	Entity::tick(level);

	if (age >= 5 * 4) {
		dead = true;
	}
}

void BlowEntity::draw(Level& level, Renderer& renderer) {
	auto& layer = renderer.terrain;
	auto& writer = *layer.writer;
	auto& tileset = *layer.tileset;

	emitEntityQuad(level, writer, tileset.sprite((int) age / 5, 2), size, angle, Color::white());
}
