#pragma once

#include <external.hpp>
#include <rendering.hpp>
#include <util/field.hpp>
#include <const.hpp>

struct ScreenTile {

	uint16_t index;  // the index of tile in the tile set
	uint16_t action; // the 'action code'
	uint16_t color;  // the color pallet offset

	ScreenTile(uint16_t index, uint16_t action, uint16_t color)
	: index(index), action(action), color(color) {}

};

struct ScreenColor {

	uint8_t fr, fg, fb, fa;
	uint8_t br, bg, bb, ba;

	ScreenColor(uint8_t fr, uint8_t fg, uint8_t fb, uint8_t fa, uint8_t br, uint8_t bg, uint8_t bb, uint8_t ba)
	: fr(fr), fg(fg), fb(fb), fa(fa), br(br), bg(bg), bb(bb), ba(ba) {}

};

class ScreenPallet {

	private:

		std::vector<ScreenColor> colors;

	public:

		uint16_t put(const ScreenColor& color) {
			colors.push_back(color);
			return colors.size() - 1;
		}

	public:

		bool has(uint16_t color) const {
			return color < colors.size();
		}

		const ScreenColor& get(uint16_t color) const {
			return colors[color];
		}

};

class Screen {

	private:

		const uint16_t actions;
		const ScreenPallet& pallet;

	protected:

		Field<ScreenTile> tiles;
		uint16_t selected = 0;
		bool closed = false;
		uint16_t width, height;

	public:

		Screen(const ScreenPallet& pallet, uint32_t width, uint32_t height, uint16_t actions)
		: pallet(pallet), tiles(width, height), actions(actions), width(width), height(height) {}

		virtual ~Screen() {}

		void render(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) {
			const auto [w, h] = gls::get_canvas_size(HTML_CANVAS);

			float ox = (w - tiles.width * 32) * 0.5f;
			float oy = (h - tiles.height * 32) * 0.5f;

			for (int y = 0; y < tiles.height; y ++) {
				for (int x = 0; x < tiles.width; x ++) {
					ScreenTile& tile = tiles.at(x, y);
					const uint16_t color = transform(tile.color, tile.action);

					if (tile.index != 0 && pallet.has(color)) {
						gls::Sprite s = tileset.sprite(tile.index);
						ScreenColor c = pallet.get(color);

						float tx = ox + x * 32.0f;
						float ty = oy + y * 32.0f;

						buffer.push({tx + 0,   0 + ty, s.min_u, s.min_v, c.fr, c.fg, c.fb, c.fa});
						buffer.push({tx + 32,  0 + ty, s.max_u, s.min_v, c.fr, c.fg, c.fb, c.fa});
						buffer.push({tx + 32, 32 + ty, s.max_u, s.max_v, c.fr, c.fg, c.fb, c.fa});
						buffer.push({tx + 32, 32 + ty, s.max_u, s.max_v, c.fr, c.fg, c.fb, c.fa});
						buffer.push({tx + 0,  32 + ty, s.min_u, s.max_v, c.fr, c.fg, c.fb, c.fa});
						buffer.push({tx + 0,   0 + ty, s.min_u, s.min_v, c.fr, c.fg, c.fb, c.fa});
					}
				}
			}
		}

		/// used for building the tilemap
		virtual void build() {

		}

		virtual bool is_closed() const {
			return closed;
		}

		/// called to close this screen
		virtual void close() {
			closed = true;
		}

		/// called when the users wants to select the next option
		virtual void next() {
			selected ++;

			if (selected > actions) {
				selected = 1;
			}
		}

		/// returns the actual color of the tile, in acordance with screen state
		virtual uint16_t transform(uint16_t color, uint16_t action) const {
			return color + (action != 0 && action == selected ? 1 : 0);
		}

		/// executed when the user triggers an action
		virtual void on_action(uint16_t action) {
			// do nothing
		}

		/// executed for each keyboard event while the screen is focused
		virtual bool on_key(int key) {
			if (key == gls::Key::ESCAPE) {
				if (selected != 0) {
					selected = 0;
				} else {
					close();
				}

				return true;
			}

			if (key == gls::Key::TAB) {
				next();
				return true;
			}

			if (key == gls::Key::ENTER && selected != 0) {
				on_action(selected);
				return true;
			}

			return false;
		}

};

class ScreenStack {

	private:

		std::list<std::shared_ptr<Screen>> screens;

	public:

		void open(std::shared_ptr<Screen> screen) {
			screens.push_back(screen);
			screen->build();
		}

		inline std::shared_ptr<Screen> top() {
			return screens.back();
		}

		void render(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) {
			if (!screens.empty()) {
				top()->render(tileset, buffer);
			}

			screens.remove_if([] (const std::shared_ptr<Screen>& screen) {
				return screen->is_closed();
			});
		}

		bool on_key(int key) {
			if (!screens.empty()) {
				return top()->on_key(key);
			}

			return false;
		}

};
