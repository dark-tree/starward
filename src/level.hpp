#pragma once

#include <external.hpp>
#include <rendering.hpp>

struct Level {

	void render(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) {

		static uint8_t tiles[6][6] = {
			{'H', 'e', 'l', 'l', 'o', '!'},
			{'H', 'e', 'l', 'l', 'o', '!'},
			{'H', 'e', 'l', 'l', 'o', '!'},
			{'H', 'e', 'l', 'l', 'o', '!'},
			{'H', 'e', 'l', 'l', 'o', '!'},
			{'H', 'e', 'l', 'l', 'o', '!'},
		};

		for (int y = 0; y < 6; y ++) {
			for (int x = 0; x < 6; x ++) {
				uint8_t tile = tiles[y][x];

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
