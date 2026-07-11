
#include "tile.hpp"

#include "game/level/level.hpp"
#include "../../level/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * TileEntity
 */

TileEntity::TileEntity(uint8_t tile, int tx, int ty)
: Entity(4, tx, ty) {
	this->tile = tile;
}

bool TileEntity::shouldCollide(Entity* entity) {
	return false;
}

void TileEntity::tick(Level& level) {
	if (age >= max_age) {
		dead = true;

		if (tile == 3) {
			level.addScore(10);
		}
	}

	age ++;
}

void TileEntity::draw(Level& level, Renderer& renderer) {
	Color color = Color::white().withAlpha(255 * (max_age - age) / max_age);
	emitEntityQuad(level, *renderer.terrain.writer, getTileSprite(*renderer.terrain.tileset, tile), size, angle, color);
}
