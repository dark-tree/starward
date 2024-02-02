#pragma once

#include <external.hpp>
#include <rendering.hpp>

class MenuScreen : public Screen {

	public:

		MenuScreen(const ScreenPallet& pallet)
		: Screen(pallet, 5, 5, 2) {}

	private:

		void build() override {
			//printf("building screen %p\n", this);

			for (int i = 0; i < 5; i ++) {
				for (int j = 0; j < 5; j ++) {
					ScreenTile& tile = tiles.at(i, j);

					tile.index = '+';
					tile.action = 100;
					tile.color = 0;
				}
			}

			for (int i = 1; i < 4; i ++) {
				for (int j = 1; j < 4; j ++) {
					ScreenTile& tile = tiles.at(i, j);

					tile.index = '=';
					tile.action = 2 - (j - 1);
					tile.color = 0;
				}
			}


		}

		void on_action(uint16_t action) override {
			printf("Selected action %d\n", (int) action);
		}

};
