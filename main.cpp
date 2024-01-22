#include <functional>

#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "src/wrapper.hpp"
#include "src/shader.hpp"
#include "src/buffer.hpp"
#include "src/texture.hpp"
#include "src/framebuffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/image.h"

// docs
// https://emscripten.org/docs/api_reference/html5.h.html

// Shader sources
const char* vertex_source = R"(#version 300 es
	in vec2 iPos;
	in vec2 iTex;
	in vec3 iCol;

	out vec2 vTex;
	out vec3 vCol;
	
	void main() {
		gl_Position = vec4(iPos.xy, 1.0, 1.0);
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

EM_BOOL mouse_handler (int eventType, const EmscriptenMouseEvent *keyEvent, void *userData) {
	mx = keyEvent->clientX;
	my = keyEvent->clientY;

	return true;
}

int main() {

	float vertices[] = {
		 0.0f,  0.5f,   0.5, 1.0,   0.60, 0.85, 0.30,
		 0.5f, -0.5f,   1.0, 0.0,   0.85, 0.35, 0.30,
		-0.5f, -0.5f,   0.0, 0.0,   0.35, 0.35, 0.30,
	};
    

	float vertices_quad[] = {
		-0.9, -0.9, 0.1, 0.1, 0.9, 0.9, 0.9,
		 0.9, -0.9, 0.9, 0.1, 0.9, 0.9, 0.9,
		 0.9,  0.9, 0.9, 0.9, 0.9, 0.9, 0.9,

		 0.9,  0.9, 0.9, 0.9, 0.9, 0.9, 0.9,
		-0.9,  0.9, 0.1, 0.9, 0.9, 0.9, 0.9,
		-0.9, -0.9, 0.1, 0.1, 0.9, 0.9, 0.9,
	};

	int w, h;

    gls::webgl_init();
	emscripten_get_canvas_element_size("#canvas", &w, &h);

	emscripten_set_mousemove_callback("#canvas", nullptr, false, mouse_handler);

	const gls::Framebuffer& frame_0 = gls::Framebuffer::main();

	gls::Texture color_att;
	color_att.resize(w, h, GL_RGB, GL_RGB);

	gls::RenderBuffer depth_att;
	depth_att.resize(w, h, GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8);
	
	gls::Framebuffer frame_1;
	frame_1.attach(color_att, GL_COLOR_ATTACHMENT0);
	frame_1.attach(depth_att, GL_DEPTH_STENCIL_ATTACHMENT);
	
	int width, height, channels;
	unsigned char* data = stbi_load("assets/test.png", &width, &height, &channels, 4);
	
	if (data == nullptr) {
		exit(-1);
	}

	gls::Texture texture;
	texture.upload(data, width, height, 4);

	stbi_image_free(data);


	// Create and compile the shader program
	gls::Shader shader {vertex_source, fragment_source};
	shader.use();
	
	// Create buffer layout
	gls::Layout layout;
	layout.attribute(shader.attribute("iPos"), 2, GL_FLOAT);
	layout.attribute(shader.attribute("iTex"), 2, GL_FLOAT);
	layout.attribute(shader.attribute("iCol"), 3, GL_FLOAT);

	// Create buffer
	gls::Buffer trig_buffer {layout, GL_DYNAMIC_DRAW};
	gls::Buffer quad_buffer {layout, GL_STATIC_DRAW};

	quad_buffer.upload((uint8_t*) vertices_quad, sizeof(vertices_quad));

	gls::main_loop([&] {

		const uint32_t milliseconds_since_start = emscripten_get_now();
        const uint32_t milliseconds_per_loop = 3000;

		vertices[0] = (mx * 2 / w) - 1;
		vertices[1] = ((h - my) * 2 / h) - 1;

		vertices[2] = vertices[0] + 0.5f;
		vertices[3] = vertices[1] + 0.5f;

		trig_buffer.upload((uint8_t*) vertices, sizeof(vertices));

		// render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		frame_1.use();
		frame_1.clear();
		texture.use();
		trig_buffer.draw();

		glClearColor(0.0f, 0.2f, 0.0f, 1.0f);
		frame_0.use();
		frame_0.clear();
		color_att.use();
		quad_buffer.draw();

	});

    return EXIT_SUCCESS;
}


