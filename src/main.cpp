
#include <external.hpp>
#include <rendering.hpp>

#include "level.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// docs
// https://emscripten.org/docs/api_reference/html5.h.html

// Shader sources
const char* vertex_source = R"(#version 300 es
	uniform mat4 matrix;

	in vec2 iPos;
	in vec2 iTex;
	in vec4 iCol;

	out vec2 vTex;
	out vec4 vCol;

	void main() {
		gl_Position = matrix * vec4(iPos.xy, 1.0, 1.0);
		vTex = iTex;
		vCol = iCol;
	}
)";

const char* fragment_source = R"(#version 300 es
	precision mediump float;
	uniform sampler2D sampler;

	in vec2 vTex;
	in vec4 vCol;

	out vec4 fColor;

	void main() {
		fColor = vCol * texture(sampler, vTex);
	}
)";

// the function called by the javascript code
extern "C" void EMSCRIPTEN_KEEPALIVE toggle_background_color() {

}

float mx, my;

EM_BOOL mouse_handler (int type, const EmscriptenMouseEvent* event, void* userdata) {
	mx = event->clientX;
	my = event->clientY;

	return true;
}

int main() {

	gls::Vert4f4b vertices[] = {
		{-0.5f,  0.5f,  0.0,  1.0,  255, 255, 255, 255},
		{ 0.5f, -0.5f,  1.0,  0.0,  255, 255, 255, 255},
		{-0.5f, -0.5f,  0.0,  0.0,  255, 255, 255, 255},
	};

	gls::Vert4f4b vertices_quad[] = {
		{-0.9, -0.9,  0.1,  0.1,  255, 255, 255, 255},
		{ 0.9, -0.9,  0.9,  0.1,  255, 255, 255, 255},
		{ 0.9,  0.9,  0.9,  0.9,  255, 255, 255, 255},

		{ 0.9,  0.9,  0.9,  0.9,  255, 255, 255, 255},
		{-0.9,  0.9,  0.1,  0.9,  255, 255, 255, 255},
		{-0.9, -0.9,  0.1,  0.1,  255, 255, 255, 255},
	};

	int32_t w, h;

	stbi_set_flip_vertically_on_load(true);
    gls::webgl_init();
	emscripten_get_canvas_element_size("#canvas", &w, &h);
	emscripten_set_mousemove_callback("#canvas", nullptr, false, mouse_handler);
	emscripten_set_keydown_callback("#canvas", nullptr, false, gls::__keydown_handler);
	emscripten_set_keyup_callback("#canvas", nullptr, false, gls::__keyup_handler);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), w / (float) h, 0.1f, 100.0f);

	const float sx = 2.0f / w;
	const float sy = 2.0f / h;

	glm::mat4 screen_space_matrix {
		 sx,  0,   0,   0,
		 0,   sy,  0,   0,
		 0,   0,   1,   0,
		-1,  -1,   0,   1,
	};

	const gls::Framebuffer& frame_0 = gls::Framebuffer::main();

	gls::Texture color_att;
	color_att.resize(w, h, GL_RGB, GL_RGB);

	gls::RenderBuffer depth_att;
	depth_att.resize(w, h, GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8);

	gls::Texture bricks {"assets/test.png"};

	// Create and compile the shader program
	gls::Shader shader {vertex_source, fragment_source};
	shader.use();

	// Create buffer layout
	gls::Layout layout;
	layout.attribute(shader.attribute("iPos"), 2, GL_FLOAT);
	layout.attribute(shader.attribute("iTex"), 2, GL_FLOAT);
	layout.attribute(shader.attribute("iCol"), 4, GL_UNSIGNED_BYTE, true);

	// Create buffers
	gls::Buffer trig_buffer {layout, GL_DYNAMIC_DRAW};
	trig_buffer.upload((uint8_t*) vertices, sizeof(vertices));

	gls::Buffer quad_buffer {layout, GL_STATIC_DRAW};
	quad_buffer.upload((uint8_t*) vertices_quad, sizeof(vertices_quad));

	gls::TileSet font8x8 {"assets/font8x8.png", 8};
	gls::TileSet tileset {"assets/tileset.png", 16};

	gls::Buffer sprite_buf {layout, GL_STATIC_DRAW};
	gls::BufferWriter<gls::Vert4f4b> writer {sprite_buf};

	World world;

	for (int i = 0; i < 20; i ++) {
		world.set(i, 0, 10);
	}

	// platform 1
	world.set(4, 5, 9);
	world.set(5, 5, 9);
	world.set(6, 5, 9);

	// platform 2
	world.set(9, 5, 9);
	world.set(10, 5, 9);
	world.set(11, 5, 9);

	// wall
	world.set(17, 2, 9);
	world.set(17, 3, 9);
	world.set(17, 4, 9);

	Level level {world};
	level.entites.push_back({});

	printf("System ready!\n");

	gls::main_loop([&] {

		glm::mat4 matrix;

		// render with perspective
//		matrix = projection * view * model;
		matrix = screen_space_matrix;
		glUniformMatrix4fv(shader.uniform("matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

		level.render(tileset, writer);
		writer.upload();

		// render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		frame_0.use();
		frame_0.clear();
		tileset.use();
		sprite_buf.draw();
		bricks.use();
		trig_buffer.draw();

	});

    return EXIT_SUCCESS;
}
