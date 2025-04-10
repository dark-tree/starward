
#include "blow.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * BlowEntity
 */

BlowEntity::BlowEntity(double x, double y)
: Entity(1, 64, x, y) {}

bool BlowEntity::shouldCollide(Entity* entity) {
	return false;
}

gls::Sprite BlowEntity::sprite(gls::TileSet& tileset) {
	return tileset.sprite((int) age / 5, 2);
}

void BlowEntity::tick(Level& level) {
	Entity::tick(level);

	if (age >= 5 * 4) {
		dead = true;
	}
}
