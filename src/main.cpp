
#include <external.hpp>
#include <rendering.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// docs
// https://emscripten.org/docs/api_reference/html5.h.html

// Shader sources
const char* vertex_source = R"(#version 300 es
	uniform mat4 matrix;

	in vec2 iPos;
	in vec2 iTex;
	in vec3 iCol;

	out vec2 vTex;
	out vec3 vCol;

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
	in vec3 vCol;

	out vec4 fColor;

	void main() {
		fColor = vec4(vCol.rgb, 0) * texture(sampler, vTex);
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

	float vertices[] = {
		-0.5f,  0.5f,  0.0,  1.0,  0.60,  0.85,  0.30,
		 0.5f, -0.5f,  1.0,  0.0,  0.85,  0.35,  0.30,
		-0.5f, -0.5f,  0.0,  0.0,  0.35,  0.35,  0.30,
	};

	float vertices_quad[] = {
		-0.9, -0.9,  0.1,  0.1,  0.9,  0.9,  0.9,
		 0.9, -0.9,  0.9,  0.1,  0.9,  0.9,  0.9,
		 0.9,  0.9,  0.9,  0.9,  0.9,  0.9,  0.9,

		 0.9,  0.9,  0.9,  0.9,  0.9,  0.9,  0.9,
		-0.9,  0.9,  0.1,  0.9,  0.9,  0.9,  0.9,
		-0.9, -0.9,  0.1,  0.1,  0.9,  0.9,  0.9,
	};

	int32_t w, h;

	stbi_set_flip_vertically_on_load(true);
    gls::webgl_init();
	emscripten_get_canvas_element_size("#canvas", &w, &h);
	emscripten_set_mousemove_callback("#canvas", nullptr, false, mouse_handler);

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
	layout.attribute(shader.attribute("iCol"), 3, GL_FLOAT);

	// Create buffers
	gls::Buffer trig_buffer {layout, GL_DYNAMIC_DRAW};
	trig_buffer.upload((uint8_t*) vertices, sizeof(vertices));

	gls::Buffer quad_buffer {layout, GL_STATIC_DRAW};
	quad_buffer.upload((uint8_t*) vertices_quad, sizeof(vertices_quad));

	gls::TileSet font8x8 {"assets/font8x8.png", 8, 8};
//	gls::Sprite s = font8x8.sprite(0, 4);

	std::vector<float> vertices_glyph;

	auto push = [&] (float x, float y, float u, float v) {
		vertices_glyph.push_back(x);
		vertices_glyph.push_back(y);
		vertices_glyph.push_back(u);
		vertices_glyph.push_back(v);
		vertices_glyph.push_back(1);
		vertices_glyph.push_back(1);
		vertices_glyph.push_back(1);
	};

	auto sprite = [&] (float x, float y, int sx, int sy) {
		gls::Sprite s = font8x8.sprite(sx, sy);

		push(x + 0,   0 + y,  s.min_u, s.min_v);
		push(x + 64,  0 + y,  s.max_u, s.min_v);
		push(x + 64, 64 + y,  s.max_u, s.max_v);
		push(x + 64, 64 + y,  s.max_u, s.max_v);
		push(x + 0,  64 + y,  s.min_u, s.max_v);
		push(x + 0,   0 + y,  s.min_u, s.min_v);
	};

	int i = 0;
	sprite(i * 64, 0, 0, 6); i ++;
	sprite(i * 64, 0, 5, 7); i ++;
	sprite(i * 64, 0, 4, 6); i ++;
	sprite(i * 64, 0, 4, 6); i ++;
	sprite(i * 64, 0, 7, 6); i ++;

	gls::Buffer sprite_buf {layout, GL_STATIC_DRAW};
	sprite_buf.upload((uint8_t*) vertices_glyph.data(), vertices_glyph.size() * sizeof(float));

	printf("System ready!\n");

	gls::main_loop([&] {

		glm::mat4 matrix;

		// render with perspective
//		matrix = projection * view * model;
		matrix = screen_space_matrix;
		glUniformMatrix4fv(shader.uniform("matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

		// render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		frame_0.use();
		frame_0.clear();
		font8x8.use();
		sprite_buf.draw();
		bricks.use();
		trig_buffer.draw();

	});

    return EXIT_SUCCESS;
}
