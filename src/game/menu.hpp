#pragma once

#include <external.hpp>
#include <rendering.hpp>

class MenuScreen : public gls::Screen {

	public:

		MenuScreen(const gls::ScreenPallet& pallet)
		: Screen(pallet, 10, 10, 3) {}

	private:

		void build() override {
			build_frame("Menu");

			build_string(3, 6, 4, "Play", 1);
			build_string(3, 4, 4, "Test", 2);
			build_string(3, 2, 4, "Exit", 3);
		}

		void on_action(uint16_t action) override {
			printf("Selected action %d\n", (int) action);
			close();
		}

};
