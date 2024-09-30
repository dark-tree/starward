
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

const char* postprocess_vertex_source = R"(#version 300 es

	in vec2 iPos;
	in vec2 iTex;

	out vec2 fragCoord;

	void main() {
		fragCoord = iPos;
		gl_Position = vec4(iPos.xy, 1.0, 1.0);
	}
)";

const char* postprocess_fragment_source = R"(#version 300 es
	precision mediump float;
	uniform sampler2D iChannel0;
	uniform float iTime;
	uniform vec2 iResolution;

	in vec2 fragCoord;
	out vec4 fragColor;

	vec2 curve(vec2 uv) {
		uv = (uv - 0.5) * 2.0;
		uv *= 1.1;
		uv.x *= 1.0 + pow((abs(uv.y) / 5.0), 2.0);
		uv.y *= 1.0 + pow((abs(uv.x) / 4.0), 2.0);
		uv  = (uv / 2.0) + 0.5;
		uv =  uv *0.92 + 0.04;
		return uv;
	}

	void main() {
		vec2 q = (fragCoord.xy + 1.0f) / 2.0f;
		vec2 uv = q;
		uv = curve( uv );
		vec3 oricol = texture( iChannel0, vec2(q.x,q.y) ).xyz;
		vec3 col;
		float x =  sin(0.3*iTime+uv.y*21.0)*sin(0.7*iTime+uv.y*29.0)*sin(0.3+0.33*iTime+uv.y*31.0)*0.0017;

		col.r = texture(iChannel0,vec2(x+uv.x+0.001,uv.y+0.001)).x+0.05;
		col.g = texture(iChannel0,vec2(x+uv.x+0.000,uv.y-0.002)).y+0.05;
		col.b = texture(iChannel0,vec2(x+uv.x-0.002,uv.y+0.000)).z+0.05;
		col.r += 0.08*texture(iChannel0,0.75*vec2(x+0.025, -0.027)+vec2(uv.x+0.001,uv.y+0.001)).x;
		col.g += 0.05*texture(iChannel0,0.75*vec2(x+-0.022, -0.02)+vec2(uv.x+0.000,uv.y-0.002)).y;
		col.b += 0.08*texture(iChannel0,0.75*vec2(x+-0.02, -0.018)+vec2(uv.x-0.002,uv.y+0.000)).z;

		col = clamp(col*0.6+0.4*col*col*1.0,0.0,1.0);

		float vig = (0.0 + 1.0*16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y));
		col *= vec3(pow(vig,0.3));

		col *= vec3(0.95,1.05,0.95);
		col *= 2.8;

		float scans = clamp( 0.35+0.35*sin(3.5*iTime+uv.y*iResolution.y*1.5), 0.0, 1.0);

		float s = pow(scans,1.7);
		col = col*vec3( 0.4+0.7*s) ;

		col *= 1.0+0.01*sin(110.0*iTime);
		if (uv.x < 0.0 || uv.x > 1.0)
			col *= 0.0;
		if (uv.y < 0.0 || uv.y > 1.0)
			col *= 0.0;

		col*=1.0-0.65*vec3(clamp((mod(q.x * iResolution.x, 2.0)-1.0)*2.0,0.0,1.0));

		float comp = smoothstep( 0.1, 0.9, sin(iTime) );

		// Remove the next line to stop cross-fade between original and postprocess
	//	col = mix( col, oricol, comp );

		fragColor = vec4(col,1.0);
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

	gls::Vert4f vertices_quad[] = {
		{-1, -1, 0, 0},
		{+1, -1, 1, 0},
		{+1, +1, 1, 1},

		{+1, +1, 1, 1},
		{-1, +1, 0, 1},
		{-1, -1, 0, 0},
	};

    gls::init();
	SoundSystem::getInstance();
	Sounds::load();

	Level level;
	level.addEntity(new PlayerEntity {});
	level.addEntity(new SweeperAlienEntity {100, 400});

	gls::Framebuffer pass_1;
	const gls::Framebuffer& pass_2 = gls::Framebuffer::main();

	gls::Texture color_att;
	gls::RenderBuffer depth_att;

	// Create and compile the shader program
	gls::Shader shader {vertex_source, fragment_source};
	gls::Shader postprocessing {postprocess_vertex_source, postprocess_fragment_source};

	shader.use();

	// Create buffer layout
	gls::Layout geometry_layout;
	geometry_layout.attribute(shader.attribute("iPos"), 2, GL_FLOAT);
	geometry_layout.attribute(shader.attribute("iTex"), 2, GL_FLOAT);
	geometry_layout.attribute(shader.attribute("iCol"), 4, GL_UNSIGNED_BYTE, true);

	postprocessing.use();

	gls::Layout screen_layout;
	screen_layout.attribute(postprocessing.attribute("iPos"), 2, GL_FLOAT);
	screen_layout.attribute(postprocessing.attribute("iTex"), 2, GL_FLOAT);

	gls::Buffer quad_buffer {screen_layout, GL_STATIC_DRAW};
	quad_buffer.upload((uint8_t*) vertices_quad, sizeof(vertices_quad));

	gls::TileSet font8x8 {"assets/font8x8.png", 8};
	gls::TileSet tileset {"assets/tileset.png", 16};

	gls::Buffer geometry_buffer {geometry_layout, GL_DYNAMIC_DRAW};
	gls::BufferWriter<gls::Vert4f4b> writer {geometry_buffer};

	printf("System ready!\n");

	gls::main_loop([&] {

		level.tick();

		// takes care of the screen ratio, calls the callback when the screen resizes
		checkViewport(ASPECT_RATIO, [&] (int w, int h, glm::mat4& matrix) {
			shader.use();
			glUniformMatrix4fv(shader.uniform("matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

			postprocessing.use();
			glUniform1f(postprocessing.uniform("iTime"), 100);
			glUniform2f(postprocessing.uniform("iResolution"), (float) w, (float) h);

			color_att.resize(w, h, GL_RGBA, GL_RGBA);
			depth_att.resize(w, h, GL_DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8);

			color_att.use();
			pass_1.attach(color_att, GL_COLOR_ATTACHMENT0);

			depth_att.use();
			pass_1.attach(depth_att, GL_DEPTH_STENCIL_ATTACHMENT);
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
		pass_1.use();
		pass_1.clear();
		tileset.use();

		shader.use();

		gls::scissor(true);
		gls::blend(true);
		geometry_buffer.draw();
		gls::scissor(false);

		pass_2.use();
		pass_2.clear();
		color_att.use();
		postprocessing.use();
		quad_buffer.draw();

		SoundSystem::getInstance().update();
		gls::Input::clear();

	});

    return EXIT_SUCCESS;
}
