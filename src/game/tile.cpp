
#include "tile.hpp"

gls::Sprite getTileSprite(gls::TileSet& tileset, uint8_t tile) {
	return tileset.sprite(tile, 4);
}