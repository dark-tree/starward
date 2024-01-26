#pragma once

#include "world.hpp"

int key_code = 0;

struct Entity {

	protected:

		float x = 100, y = 100;
		float vx = 0, vy = 0;

	public:

		virtual ~Entity() {};
		virtual void render(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) = 0;
		virtual void tick(World& world) = 0;

};


class Player : public Entity {

	protected:

		bool on_ground = false;
		float ix = 0, iy = 0;

	public:

		void render(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) override {
			gls::Sprite s = tileset.sprite(11);

			uint8_t v = on_ground ? 255 : 100;

			buffer.push({x + 0,   0 + y,  s.min_u, s.min_v, 255, 255, v, 255});
			buffer.push({x + 64,  0 + y,  s.max_u, s.min_v, 255, 255, v, 255});
			buffer.push({x + 64, 64 + y,  s.max_u, s.max_v, 255, 255, v, 255});
			buffer.push({x + 64, 64 + y,  s.max_u, s.max_v, 255, 255, v, 255});
			buffer.push({x + 0,  64 + y,  s.min_u, s.max_v, 255, 255, v, 255});
			buffer.push({x + 0,   0 + y,  s.min_u, s.min_v, 255, 255, v, 255});
		}

		void tick(World& world) override {
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

			x += 32;

			int x1 = x / 64;
			int y1 = (y + iy + vy) / 64;

			if (world.get(x1, y1) != 0) {
				if (vy < 0) {
					on_ground = true;
				}

				vy /= 2;
			} else {
				y = y + iy + vy;
			}

			int x2 = (x + ix + vx) / 64;
			int y2 = y / 64;

			if (world.get(x2, y2) != 0) {
				vx /= 2;
			} else {
				x = x + ix + vx;
			}

			if (gls::Input::is_pressed(gls::Key::LEFT)) {
				vx = -5;
			} else if (gls::Input::is_pressed(gls::Key::RIGHT)) {
				vx = +5;
			} else {
				if (on_ground) vx /= 2;
			}

			if ((gls::Input::is_pressed(gls::Key::SPACE) || gls::Input::is_pressed(gls::Key::UP)) && on_ground) {
				on_ground = false;
				vy += 12;
			}

			x -= 32;
		}

};
