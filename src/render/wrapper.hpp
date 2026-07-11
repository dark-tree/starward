#pragma once

#include <external.hpp>

extern int render_region_offset_x;
extern int render_region_offset_y;
extern int render_region_width;
extern int render_region_height;

void __main_loop();

struct Rectangle {
	int width;
	int height;
};

/// Start OpenGL/WebGL & input context
void initVideoSystem();

/// Return window/canvas size in pixels
Rectangle getCanvasSize();

/// Set the render function to use
void setMainLoop(const std::function<void()>& loop);