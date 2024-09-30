
#include <external.hpp>
#include <rendering.hpp>

#include "const.hpp"
#include "game/sounds.hpp"
#include "game/entity.hpp"
#include "game/level.hpp"

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
		fColor = vCol.rgba * texture(sampler, vTex).rgba;
	}
)";

// the function called by the javascript code
extern "C" void EXPORTED_NATIVE toggle_background_color() {

}

void checkViewport(float ratio, const std::function<void(int, int, glm::mat4& matrix)>& on_resize) {

	static int pw = 0;
	static int ph = 0;

	const auto [w, h] = gls::get_canvas_size();

	if (w != pw || h != ph) {

		const float sx = 2.0f / w;
		const float sy = 2.0f / h;

		float rw = 0;
		float rh = 0;

		if (w > h * ratio) {
			rh = h;
			rw = h * ratio;
		} else {
			rh = w / ratio;
			rw = w;
		}

		// margin
		const float mx = (w - rw) / 2.0f;
		const float my = (h - rh) / 2.0f;

		// offset
		const float ox = sx * mx - 1;
		const float oy = sy * my - 1;

		// factor
		const float fx = sx * (rw / SW);
		const float fy = sy * (rh / SH);

		glm::mat4 matrix {
			fx,   0,   0,   0,
			 0,  fy,   0,   0,
			 0,   0,   1,   0,
			ox,  oy,   0,   1,
		};

		pw = w;
		ph = h;

		gls::setViewportArea(w, h);
		gls::setScissorArea(mx, my, rw, rh);

		on_resize(w, h, matrix);

		printf("Screen resized to %dx%d\n", w, h);

	}

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

    gls::init();
	SoundSystem::getInstance();
	Sounds::load();

	Level level;
	level.addEntity(new PlayerEntity {});
	level.addEntity(new SweeperAlienEntity {100, 400});

	const gls::Framebuffer& frame_0 = gls::Framebuffer::main();

	gls::Texture color_att;
	gls::RenderBuffer depth_att;

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

	printf("System ready!\n");

	int w43, h43;

	gls::main_loop([&] {

		level.tick();

		// takes care of the screen ratio, calls the callback when the screen resizes
		checkViewport(ASPECT_RATIO, [&] (int w, int h, glm::mat4& matrix) {
			glUniformMatrix4fv(shader.uniform("matrix"), 1, GL_FALSE, glm::value_ptr(matrix));
			color_att.resize(w, h, GL_RGB, GL_RGB);
			depth_att.resize(w, h, GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8);
		});

		int tx = 0;
		int ty = 0;

		gls::Sprite s {0, 0, 1, 1};

//		writer.push({tx + 0, 0 + ty,  s.min_u, s.min_v, 255, 255, 255, 255});
//		writer.push({tx + SW, 0 + ty,  s.max_u, s.min_v, 255, 255, 255, 255});
//		writer.push({tx + SW, SH + ty,  s.max_u, s.max_v, 255, 255, 255, 255});
//		writer.push({tx + SW, SH + ty,  s.max_u, s.max_v, 255, 255, 255, 255});
//		writer.push({tx + 0, SH + ty,  s.min_u, s.max_v, 255, 255, 255, 255});
//		writer.push({tx + 0, 0 + ty,  s.min_u, s.min_v, 255, 255, 255, 255});

		level.draw(tileset, writer);

//		s = tileset.sprite(0, 0);
//
//		for (int x = 0; x < 64; x++) {
//			for (int y = 0; y < 64; y++) {
//				if (rand() % 100 < 2) {
//					drawTile(writer, s, x, y, 64);
//				}
//			}
//		}

		writer.upload();

		// render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		frame_0.use();
		frame_0.clear();
		tileset.use();

		gls::scissor(true);
		gls::blend(true);
		sprite_buf.draw();
		gls::scissor(false);

		SoundSystem::getInstance().update();
		gls::Input::clear();

	});

    return EXIT_SUCCESS;
}
