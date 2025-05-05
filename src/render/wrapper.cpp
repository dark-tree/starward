#include "wrapper.hpp"

static std::function<void()> __main_loop_func;

void __main_loop() {
	__main_loop_func();
}

void initVideoSystem() {
	platform_init();

	// configure STB image
	stbi_set_flip_vertically_on_load(true);

	// set event handlers
	platform_set_keydown_callback(Input::press);
	platform_set_keyup_callback(Input::release);
}

Rectangle getCanvasSize() {
	Rectangle rect;
	platform_get_canvas_element_size(&rect.width, &rect.height);
	return rect;
}

void setMainLoop(const std::function<void()>& loop) {
	__main_loop_func = loop;
	platform_set_main_loop(__main_loop, 0);
}