
#include "tile.hpp"

Sprite getTileSprite(TileSet& tileset, uint8_t tile) {
	return tileset.sprite(tile, 4);
}