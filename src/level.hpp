#pragma once

#include <external.hpp>
#include <rendering.hpp>

struct World {

	uint16_t tiles[20 * 20];

	World() {
		memset(tiles, 0, 20 * 20 * sizeof(uint16_t));
	}

	void set(int x, int y, uint16_t tid) {
		tiles[y * 20 + x] = tid;
	}

};


#include "entity.hpp"

struct Level {

	std::vector<Entity> entites;
	World& world;

	Level(World& world)
	: world(world) {}

	void render(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) {

		for (Entity& entity : entites) {
			entity.tick(world);
		}

		for (Entity& entity : entites) {
			entity.render(tileset, buffer);
		}

		for (int y = 0; y < 20; y ++) {
			for (int x = 0; x < 20; x ++) {
				uint8_t tile = world.tiles[y * 20 + x];

				if (tile != 0) {
					gls::Sprite s = tileset.sprite(tile);

					float tx = x * 64.0f;
					float ty = y * 64.0f;

					buffer.push({tx + 0,   0 + ty,  s.min_u, s.min_v, 255, 255, 255, 255});
					buffer.push({tx + 64,  0 + ty,  s.max_u, s.min_v, 255, 255, 255, 255});
					buffer.push({tx + 64, 64 + ty,  s.max_u, s.max_v, 255, 255, 255, 255});
					buffer.push({tx + 64, 64 + ty,  s.max_u, s.max_v, 255, 255, 255, 255});
					buffer.push({tx + 0,  64 + ty,  s.min_u, s.max_v, 255, 255, 255, 255});
					buffer.push({tx + 0,   0 + ty,  s.min_u, s.min_v, 255, 255, 255, 255});
				}
			}
		}
	}

};
