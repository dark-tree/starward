#include "wrapper.hpp"
#include "input.hpp"

static std::function<void()> __main_loop_func;

void __main_loop() {
	__main_loop_func();
}

void initVideoSystem() {
	platform::init();

	// configure STB image
	stbi_set_flip_vertically_on_load(true);

	// set event handlers
	platform::set_keydown_callback(Input::press);
	platform::set_keyup_callback(Input::release);
	platform::set_touch_callback(Input::touch);
}

Rectangle getCanvasSize() {
	Rectangle rect;
	platform::get_render_target_size(&rect.width, &rect.height);
	return rect;
}

void setMainLoop(const std::function<void()>& loop) {
	__main_loop_func = loop;
	platform::set_main_loop(__main_loop, 0);
}