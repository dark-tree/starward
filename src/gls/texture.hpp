#pragma once

#include <external.hpp>
#include <util.hpp>

namespace gls {

	class PixelBuffer {

		private:

			friend class Framebuffer;
			virtual void framebuffer(GLenum attachment) const = 0;

		public:

			virtual ~PixelBuffer() {}
			virtual void resize(int w, int h, GLenum internal_format, GLenum format) = 0;

			virtual void use() const = 0;
			virtual uint32_t width() const = 0;
			virtual uint32_t height() const = 0;

	};

	class Texture : public PixelBuffer {

		protected:

			friend class TileSet;

			uint32_t tid = 0;
			uint32_t w = 0;
			uint32_t h = 0;

			GLenum format_of(int channels) {
				switch (channels) {
					case 4: return GL_RGBA;
					case 3: return GL_RGB;
					case 1: return GL_ALPHA;
				}

				fault("Unsuported texture channel count: %d!\n", channels);
			}

			void framebuffer(GLenum attachment) const override {
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tid, 0);
			}

		public:

			Texture(const char* path)
			: Texture() {
				int32_t width, height, channels;
				uint8_t* data = stbi_load(path, &width, &height, &channels, 4);

				if (data == nullptr) {
					fault("Failed to load texture: '%s'!\n", path);
				}

				upload(data, width, height, 4);
				stbi_image_free(data);
			}

			Texture() {
				glGenTextures(1, &tid);
				use();

				// set texture wrapping
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				// set texture filtering
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}

			~Texture() {
				glDeleteTextures(1, &tid);
			}

			void upload(unsigned char* data, int width, int height, int channels) {
				use();
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format_of(channels), GL_UNSIGNED_BYTE, data);
				w = width;
				h = height;
			}

			void resize(int width, int height, GLenum internal_format, GLenum format) override {
				use();
				glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
				w = width;
				h = height;
			}

			void use() const override {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tid);
			}

			uint32_t width() const override {
				return w;
			}

			uint32_t height() const override {
				return h;
			}

	};

	class RenderBuffer : public PixelBuffer {

		private:

			uint32_t rbo = 0;
			uint32_t w = 0;
			uint32_t h = 0;

			void framebuffer(GLenum attachment) const override {
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
			}

		public:

			RenderBuffer() {
				glGenRenderbuffers(1, &rbo);
				glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			}

			~RenderBuffer() {
				glDeleteRenderbuffers(1, &rbo);
			}

			void resize(int width, int height, GLenum internal_format, GLenum format) override {
				use();
				glRenderbufferStorage(GL_RENDERBUFFER, internal_format, width, height);
				w = width;
				h = height;
			}

			void use() const override {
				glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			}

			uint32_t width() const override {
				return w;
			}

			uint32_t height() const override {
				return h;
			}

	};

	class Sprite {

		public:

			float min_u, min_v, max_u, max_v;

		public:

			Sprite(float min_u, float min_v, float max_u, float max_v)
			: min_u(min_u), min_v(min_v), max_u(max_u), max_v(max_v) {
				printf("min_u=%f min_v=%f max_u=%f max_v=%f\n", min_u, min_v, max_u, max_v);
			}

	};

	class TileSet : public PixelBuffer {

		private:

			Texture texture;
			int tw, th, line;

			void framebuffer(GLenum attachment) const override {
				texture.framebuffer(attachment);
			}

		public:

			TileSet(const char* path, uint32_t tile)
			: TileSet(path, tile, tile) {}

			TileSet(const char* path, uint32_t tw, uint32_t th)
			: texture(path), tw(tw), th(th), line(texture.width() / tw) {
				if (texture.width() % tw != 0) {
					fault("Unable to neatly divide tileset! Texture width: %d tile width: %d\n", texture.width(), tw);
				}
			}

			void resize(int w, int h, GLenum internal_format, GLenum format) override {
				fault("Can't resize tileset, operation forbidden!");
			}

			void use() const override {
				texture.use();
			}

			uint32_t width() const override {
				return texture.width();
			}

			uint32_t height() const override {
				return texture.height();
			}

			uint32_t tiles_per_line() const {
				return line;
			}

		public:

			Sprite sprite(int x, int y) {
				const int min_x = x * tw;
				const int min_y = y * th;
				const int max_x = min_x + tw;
				const int max_y = min_y + th;

				return {
					(min_x + 0.5f) / (float) width(),
					(min_y + 0.5f) / (float) height(),
					(max_x + 0.5f) / (float) width(),
					(max_y + 0.5f) / (float) height()
				};
			}

	};

}
