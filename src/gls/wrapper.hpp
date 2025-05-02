#pragma once

#include "input.hpp"
#include <external.hpp>
#include <util.hpp>

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