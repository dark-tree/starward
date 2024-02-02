#pragma once

#include <external.hpp>
#include <rendering.hpp>

class MenuScreen : public Screen {

	public:

		MenuScreen(const ScreenPallet& pallet)
		: Screen(pallet, 10, 10, 3) {}

	private:

		void build() override {
			build_frame("Menu");

			build_string(3, 6, 4, "Play", 1);
			build_string(3, 4, 4, "Test", 2);
			build_string(3, 2, 4, "Exit", 3);
		}

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

		void build_string(int x, int y, int max, const char* str, uint16_t action) {
			int len = std::min((int) strlen(str), max);

			for (int i = 0; i < len; i ++) {
				tiles.at(x + i, y) = {(uint16_t) str[i], action, 0};
			}

		}

		void on_action(uint16_t action) override {
			printf("Selected action %d\n", (int) action);
			close();
		}

};
