#pragma once
#include "buffer.hpp"
#include "framebuffer.hpp"
#include "layout.hpp"
#include "shader.hpp"
#include "vertex.hpp"

struct RenderLayer {

	BufferWriter<Vert4f4b>* writer;
	TileSet* tileset;

	void init(BufferWriter<Vert4f4b>* writer, TileSet* tileset);

};

class Renderer {

	private:

		Framebuffer pass_1;
		Framebuffer pass_2;

		Texture color_att;
		RenderBuffer depth_att;

		Layout geometry_layout;
		Layout screen_layout;

		VertexBuffer blit_buffer;
		VertexBuffer game_buffer;
		VertexBuffer text_buffer;

		BufferWriter<Vert4f4b> game_writer;
		BufferWriter<Vert4f4b> text_writer;

		TileSet font8x8;
		TileSet tileset;

	public:

		Shader level_shader;
		Shader degrade_shader;

		RenderLayer terrain;
		RenderLayer text;

	public:

		Renderer();

		void beginDraw(const std::chrono::time_point<std::chrono::steady_clock>& begin_time, float aliveness);
		void endDraw(int vw, int vh);

};
