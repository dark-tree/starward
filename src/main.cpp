
#include <external.hpp>
#include <rendering.hpp>

#include "const.hpp"
#include "game/level.hpp"
#include "game/menu.hpp"
#include "sound/system.hpp"

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
extern "C" void EXPORTED_NATIVE toggle_background_color() {

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
	SoundSystem sounds;
	SoundBuffer buffer {"assets/test.ogg"};

	const auto [w, h] = gls::get_canvas_size();

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

	World world {30, 20};

	for (int i = 0; i < world.width; i ++) {
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
	world.set(16, 2, 9);
	world.set(13, 5, 9);

	MapCollider collider(&world);

	Level level {world};
	level.entites.push_back(std::make_shared<Player>());
	//level.entites.push_back(std::make_shared<Box>(glm::vec2(200.0f, 100.0f)));

	gls::ScreenPallet pallet;
	pallet.put({100, 120, 100, 255, 0, 0, 0, 0});
	pallet.put({200, 240, 200, 255, 0, 0, 0, 0});

	gls::ScreenStack stack;
	stack.open(std::shared_ptr<gls::Screen>{new MenuScreen {pallet}});

	printf("System ready!\n");
	bool sound_test = true;

	gls::main_loop([&] {
		Physics::tick();

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
		stack.render(font8x8, writer);
		writer.upload();
		font8x8.use();
		sprite_buf.draw();

		// ugly input wrapper, i will fix it later
		if (gls::Input::is_typed(Key::TAB)) stack.on_key(Key::TAB);
		if (gls::Input::is_typed(Key::ENTER)) stack.on_key(Key::ENTER);
		if (gls::Input::is_typed(Key::ESCAPE)) stack.on_key(Key::ESCAPE);
		if (gls::Input::is_typed(Key::UP)) stack.on_key(Key::UP);
		if (gls::Input::is_typed(Key::DOWN)) stack.on_key(Key::DOWN);

		if (sound_test && stack.empty()) {
			sounds.volume().set(SoundGroup::MUSIC, 0.5f);

			sounds.add(buffer).loop().in(SoundGroup::MUSIC).event(0.35f, [] (float current, SoundSource& source) {
				printf("From sound event of '%s', played at: '%f'\n", source.identifier(), current);
			}).play();

			sound_test = false;
		}

		sounds.update();
		gls::Input::clear();

	});

    return EXIT_SUCCESS;



}
