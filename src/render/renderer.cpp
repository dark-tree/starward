#include "renderer.hpp"

#include "state.hpp"

/*
 * RenderLayer
 */

void RenderLayer::init(BufferWriter<Vert4f4b>* writer, TileSet* tileset) {
	this->writer = writer;
	this->tileset = tileset;
}

/*
 * Renderer
 */

Renderer::Renderer() {

	Vert2f vertices_quad[] = {
		{0, 0},
		{1, 0},
		{1, 1},
		{1, 1},
		{0, 1},
		{0, 0}
	};

	initVideoSystem();

	pass_1.init();
	pass_2 = Framebuffer::main();

	color_att.init();
	color_att.resize(SW, SH, GL_RGBA, GL_RGBA);

	depth_att.init();
	depth_att.resize(SW, SH, GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8);

	color_att.use();
	pass_1.attach(color_att, GL_COLOR_ATTACHMENT0);

	depth_att.use();
	pass_1.attach(depth_att, GL_DEPTH_STENCIL_ATTACHMENT);

	// Create and compile the shader program
	level_shader.init("assets/shader/level");
	degrade_shader.init("assets/shader/degrade");

	// Create buffer layout
	geometry_layout.attribute(level_shader.attribute("iPos"), 2, GL_FLOAT);
	geometry_layout.attribute(level_shader.attribute("iTex"), 2, GL_FLOAT);
	geometry_layout.attribute(level_shader.attribute("iCol"), 4, GL_UNSIGNED_BYTE, true);
	screen_layout.attribute(degrade_shader.attribute("iPos"), 2, GL_FLOAT);

	blit_buffer.init(screen_layout, GL_STATIC_DRAW);
	blit_buffer.upload((uint8_t*) vertices_quad, sizeof(vertices_quad));

	font8x8.init("assets/font8x8.png", 8);
	tileset.init("assets/tileset.png", 16);

	game_buffer.init(geometry_layout, GL_DYNAMIC_DRAW);
	text_buffer.init(geometry_layout, GL_DYNAMIC_DRAW);

	game_writer.init(&game_buffer);
	text_writer.init(&text_buffer);

	terrain.init(&game_writer, &tileset);
	text.init(&text_writer, &font8x8);

	// enable blending
	setBlend(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// static configuration
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	printf("Render system started!\n");
}

void Renderer::beginDraw(const std::chrono::time_point<std::chrono::steady_clock>& begin_time, float aliveness) {
	degrade_shader.use();
	const auto now_time = std::chrono::steady_clock::now();
	glUniform1f(degrade_shader.uniform("uTime"), std::chrono::duration_cast<std::chrono::duration<float>>(now_time - begin_time).count());
	glUniform1f(degrade_shader.uniform("uAliveness"), aliveness);
	glViewport(0, 0, SW, SH);
}

void Renderer::endDraw(int vw, int vh) {
	game_writer.upload();
	text_writer.upload();

	// render
	pass_1.use();
	pass_1.clear();

	tileset.use();
	level_shader.use();
	game_buffer.draw();

	font8x8.use();
	text_buffer.draw();

	glViewport(0, 0, vw, vh);

	// apply a CRT-like effect and draw into back buffer
	pass_2.use();
	pass_2.clear();
	color_att.use();
	degrade_shader.use();
	blit_buffer.draw();
}