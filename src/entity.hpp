#pragma once

int key_code = 0;

struct Entity {

	float tx = 100, ty = 100;
	float vx = 0, vy = 0;
	bool on_ground = false;
	float ix = 0, iy = 0;

	void render(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) {
		gls::Sprite s = tileset.sprite(11);

		uint8_t v = on_ground ? 255 : 100;

		buffer.push({tx + 0,   0 + ty,  s.min_u, s.min_v, 255, 255, v, v});
		buffer.push({tx + 64,  0 + ty,  s.max_u, s.min_v, 255, 255, v, v});
		buffer.push({tx + 64, 64 + ty,  s.max_u, s.max_v, 255, 255, v, v});
		buffer.push({tx + 64, 64 + ty,  s.max_u, s.max_v, 255, 255, v, v});
		buffer.push({tx + 0,  64 + ty,  s.min_u, s.max_v, 255, 255, v, v});
		buffer.push({tx + 0,   0 + ty,  s.min_u, s.min_v, 255, 255, v, v});
	}

	void tick(World& world) {
		if (!on_ground) {
			if (vy > -10) vy -= 0.5;
		}

		if (vx < -60) vx = -60;
		if (vx > 60) vx = 60;
		if (vy < -60) vy = -60;
		if (vy > 60) vy = 60;

		if (vy < -10) {
			vy += 2;
		}

		float px = tx, py = ty;

		int x1 = tx / 64;
		int y1 = (ty + vy) / 64;

		if (world.tiles[x1 + y1 * 20] != 0) {
			on_ground = true;
		}

		int x2 = (tx + ix + vx) / 64;
		int y2 = (ty + iy + vy) / 64;


		if (world.tiles[x2 + y2 * 20] == 0) {
			on_ground = false;

			tx = tx + ix + vx;
			ty = ty + iy + vy;
		} else {
			vx /= 2;
			vy /= 2;
		}

		if (gls::Input::is_pressed(37)) {
			ix = -5;
		}

		else if (gls::Input::is_pressed(39)) {
			ix = +5;
		} else {
			ix = 0;
		}

		if (gls::Input::is_pressed(32) && on_ground) {
			on_ground = false;
			vy += 10;
		}
	}

};
