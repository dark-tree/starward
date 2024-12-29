
#include "tile.hpp"

#include "game/level.hpp"
#include "game/tile.hpp"
#include "game/emitter.hpp"
#include "game/sounds.hpp"

/*
 * TileEntity
 */

TileEntity::TileEntity(double x, double y, uint8_t tile, int tx, int ty)
: Entity(0, 4, tx, ty) {
	float dx = x - tx;
	float dy = y - ty;

	this->tile = tile;
	this->fx = std::clamp(1 / (1 + dx), -8.0f, 8.0f);
	this->fy = std::clamp(1 / (1 + dy), -8.0f, 8.0f);
}

gls::Sprite TileEntity::sprite(gls::TileSet& tileset) {
	return getTileSprite(tileset, tile);
}

void TileEntity::tick(Level& level) {

	this->x += fx;
	this->y += fy;

	this->fx *= 0.9f;
	this->fy *= 0.9f;

	float max_age = 10;
	this->a = 255 * (max_age - age) / max_age;

	if (age >= max_age) {
		dead = true;

		if (tile == 3) {
			level.addScore(10);
		}
	}

	age ++;
}
