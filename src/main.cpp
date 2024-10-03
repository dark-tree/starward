
#include <external.hpp>
#include <rendering.hpp>

#include "const.hpp"
#include "game/sounds.hpp"
#include "game/entity.hpp"
#include "game/level.hpp"

// docs
// https://emscripten.org/docs/api_reference/html5.h.html

// the function called by the javascript code
extern "C" void EXPORTED_NATIVE toggle_background_color() {

}

void checkViewport(float ratio, const std::function<void(int, int, int, int, glm::mat4& matrix)>& on_resize) {

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
			fx * SW,   0,   0,   0,
			 0,  fy * SH,   0,   0,
			 0,   0,   1,   0,
			ox,  oy,   0,   1,
		};

		pw = w;
		ph = h;

		on_resize(w, h, rw, rh, matrix);

		printf("Screen resized to %dx%d (with region: %dx%d)\n", w, h, (int) rw, (int) rh);

	}

}

int main() {

	auto begin_time = std::chrono::steady_clock::now();

	gls::Vert2f vertices_quad[] = {
		{0, 0},
		{1, 0},
		{1, 1},
		{1, 1},
		{0, 1},
		{0, 0}
	};

    gls::init();
	SoundSystem::getInstance();
	Sounds::load();

	Level level;
	level.addEntity(new PlayerEntity {});
	level.addEntity(new SweeperAlienEntity {100, 450, 2});

	gls::Framebuffer pass_1;
	const gls::Framebuffer& pass_2 = gls::Framebuffer::main();

	gls::Texture color_att;
	color_att.resize(SW, SH, GL_RGBA, GL_RGBA);

	gls::RenderBuffer depth_att;
	depth_att.resize(SW, SH, GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8);

	color_att.use();
	pass_1.attach(color_att, GL_COLOR_ATTACHMENT0);

	depth_att.use();
	pass_1.attach(depth_att, GL_DEPTH_STENCIL_ATTACHMENT);

	// Create and compile the shader program
	gls::Shader level_shader {"assets/shader/level"};
	gls::Shader degrade_shader {"assets/shader/degrade"};

	// Create buffer layout
	gls::Layout geometry_layout;
	geometry_layout.attribute(level_shader.attribute("iPos"), 2, GL_FLOAT);
	geometry_layout.attribute(level_shader.attribute("iTex"), 2, GL_FLOAT);
	geometry_layout.attribute(level_shader.attribute("iCol"), 4, GL_UNSIGNED_BYTE, true);

	gls::Layout screen_layout;
	screen_layout.attribute(degrade_shader.attribute("iPos"), 2, GL_FLOAT);

	gls::Buffer blit_buffer {screen_layout, GL_STATIC_DRAW};
	blit_buffer.upload((uint8_t*) vertices_quad, sizeof(vertices_quad));

	gls::TileSet font8x8 {"assets/font8x8.png", 8};
	gls::TileSet tileset {"assets/tileset.png", 16};

	gls::Buffer immediate_buffer {geometry_layout, GL_DYNAMIC_DRAW};
	gls::BufferWriter<gls::Vert4f4b> writer {immediate_buffer};

	gls::blend(true);

	printf("System ready!\n");

	int __w, __h;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gls::main_loop([&] {

		level.tick();

		// takes care of the screen ratio, calls the callback when the screen resizes
		checkViewport(ASPECT_RATIO, [&] (int w, int h, int rw, int rh, glm::mat4& matrix) {
			glm::mat4 static_matrix {
				2.0f/SW,   0,   0,   0,
				0,  2.0f/SH,   0,   0,
				0,   0,   1,   0,
				-1,  -1,   0,   1,
			};

			level_shader.use();
			glUniformMatrix4fv(level_shader.uniform("uMatrix"), 1, GL_FALSE, glm::value_ptr(static_matrix));

			degrade_shader.use();
			glUniform2f(degrade_shader.uniform("uResolution"), rw, rh);
			glUniformMatrix4fv(degrade_shader.uniform("uMatrix"), 1, GL_FALSE, glm::value_ptr(matrix));

			__w = w;
			__h = h;
		});

		degrade_shader.use();
		const auto now_time = std::chrono::steady_clock::now();
		glUniform1f(degrade_shader.uniform("uTime"), std::chrono::duration_cast<std::chrono::duration<float>>(now_time - begin_time).count());

		glViewport(0, 0, SW, SH);

		// render
		level.draw(tileset, writer);
		writer.upload();

		// render
		pass_1.use();
		pass_1.clear();
		tileset.use();

		level_shader.use();
		immediate_buffer.draw();

		glViewport(0, 0, __w, __h);

		// apply a CRT-like effect and draw into back buffer
		pass_2.use();
		pass_2.clear();
		color_att.use();
		degrade_shader.use();
		blit_buffer.draw();

		SoundSystem::getInstance().update();
		gls::Input::clear();

	});

    return EXIT_SUCCESS;
}
