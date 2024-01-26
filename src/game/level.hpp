#pragma once

#include <external.hpp>
#include <rendering.hpp>
#include "entity.hpp"

struct Level {

	std::vector<std::shared_ptr<Entity>> entites;
	World& world;

	Level(World& world)
	: world(world) {}

	void render(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) {

		for (auto& entity : entites) {
			entity->tick(world);
		}

		for (auto& entity : entites) {
			entity->render(tileset, buffer);
		}

		for (int y = 0; y < world.height; y ++) {
			for (int x = 0; x < world.width; x ++) {
				uint8_t tile = world.get(x, y);

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
