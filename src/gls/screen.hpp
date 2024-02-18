#pragma once

#include <external.hpp>
#include <rendering.hpp>
#include <util/field.hpp>
#include <const.hpp>

namespace gls {

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

	enum struct ScreenState {
		OPENING,
		OPEN,
		CLOSING,
		CLOSED,
	};

	class Screen {

		private:

			const uint16_t actions;
			const ScreenPallet& pallet;

			ScreenState state;
			uint32_t render_start = 0;
			uint32_t render_end = 0;

		protected:

			Field<ScreenTile> tiles;
			uint16_t selected = 0;
			uint16_t width, height;

		public:

			Screen(const ScreenPallet& pallet, uint32_t width, uint32_t height, uint16_t actions)
			: pallet(pallet), tiles(width, height), actions(actions), width(width), height(height), state(ScreenState::OPENING) {
				render_start = height;
				render_end = height;
			}

			virtual ~Screen() {}

			void render(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) {
				const auto [w, h] = gls::get_canvas_size(HTML_CANVAS);

				float ox = (w - tiles.width * 32) * 0.5f;
				float oy = (h - tiles.height * 32) * 0.5f;

				if (state == ScreenState::OPENING) {
					if (render_start > 0) {
						render_start --;
						render_end = height;
					} else {
						state = ScreenState::OPEN;
					}
				}

				if (state == ScreenState::CLOSING) {
					if (render_end > 0) {
						render_end --;
						render_start = 0;
					} else {
						state = ScreenState::CLOSED;
					}
				}

				for (int y = render_start; y < render_end; y ++) {
					for (int x = 0; x < width; x ++) {
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
				return state == ScreenState::CLOSED;
			}

			/// called to close this screen
			virtual void close() {
				state = ScreenState::CLOSING;
			}

			/// cycle the selectied action
			virtual void cycle() {
				selected ++;

				if (selected > actions) {
					selected = 1;
				}
			}

			/// got to the next or previous action
			virtual void next(int offset) {
				selected += offset;

				if (selected > actions) {
					selected = actions;
				}

				if (selected < 1) {
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
					cycle();
					return true;
				}

				if (key == gls::Key::ENTER && selected != 0) {
					on_action(selected);
					return true;
				}

				if (key == gls::Key::UP) {
					next(-1);
					return true;
				}

				if (key == gls::Key::DOWN) {
					next(1);
					return true;
				}

				return false;
			}

		protected:

			/// build a border with a title around the interface
			void build_frame(const char* name) {
				for (int x = 0; x < width; x ++) {
					for (int y = 0; y < height; y ++) {
						tiles.at(x, y) = {' ', 0, 0};
					}
				}

				tiles.at(0, 0) = {FONT_LEFT_BOTTOM, 0, 0};
				tiles.at(0, height - 1) = {FONT_LEFT_TOP, 0, 0};
				tiles.at(width - 1, 0) = {FONT_RIGHT_BOTTOM, 0, 0};
				tiles.at(width - 1, height - 1) = {FONT_RIGHT_TOP, 0, 0};

				for (int x = 1; x < width - 1; x ++) {
					tiles.at(x, 0) = {'-', 0, 0};
					tiles.at(x, height - 1) = {'-', 0, 0};
				}

				for (int y = 1; y < height - 1; y ++) {
					tiles.at(0, y) = {'|', 0, 0};
					tiles.at(width - 1, y) = {'|', 0, 0};
				}

				int space = width - 2;
				int len = std::min((int) strlen(name), space - 2);
				int offset = (space - len) / 2;

				tiles.at(offset, height - 1) = {FONT_LEFT_PLUG, 0, 0};;
				tiles.at(width - 1 - offset, height - 1) = {FONT_RIGHT_PLUG, 0, 0};

				for (int i = 0; i < len; i ++) {
					tiles.at(offset + i + 1, height - 1) = {(uint16_t) name[i], 0, 0};
				}
			}

			/// build a string at the given position
			void build_string(int x, int y, int max, const char* str, uint16_t action) {
				int len = std::min((int) strlen(str), max);

				for (int i = 0; i < len; i ++) {
					tiles.at(x + i, y) = {(uint16_t) str[i], action, 0};
				}
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

			bool empty() {
				return screens.empty();
			}

	};

}
