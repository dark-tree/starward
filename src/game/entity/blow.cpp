
#include "blow.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
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

void BlowEntity::draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) {
	emitEntityQuad(level, writer, tileset.sprite((int) age / 5, 2), size, angle, Color::white());
}
