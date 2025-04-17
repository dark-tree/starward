#pragma once

#include <external.hpp>
#include "sprite.hpp"

namespace gls {

	/// Generic pixel buffer
	class PixelBuffer {

		private:

			friend class Framebuffer;
			virtual void framebuffer(GLenum attachment) const = 0;

		public:

			virtual ~PixelBuffer() {}

			/// Try to resize the buffer, if possible
			virtual void resize(int w, int h, GLenum internal_format, GLenum format) = 0;

			/// Bind this pixel buffer
			virtual void use() const = 0;

			/// Get width in pixels
			virtual uint32_t width() const = 0;

			/// get height in pixels
			virtual uint32_t height() const = 0;

	};

	/// A pixel buffer that can be sampled
	class Texture : public PixelBuffer {

		protected:

			friend class TileSet;

			uint32_t tid = 0;
			uint32_t w = 0;
			uint32_t h = 0;

			/// Convert channel count to OpenGL enum
			GLenum format(int channels);

			void framebuffer(GLenum attachment) const override;

		public:

			Texture(const char* path);
			Texture();
			~Texture();

			Texture(const Texture& buffer) = delete;
			Texture(Texture&& buffer) = default;

			/// Initialize texture of given size and contents
			void upload(unsigned char* data, int width, int height, int channels);

			/// Initialize texture of given size, with undefined contents
			void resize(int width, int height, GLenum internal_format, GLenum format) override;

			/// Bind this texture
			void use() const override;

			/// Get current width in pixels
			uint32_t width() const override;

			/// Get current height in pixels
			uint32_t height() const override;

	};

	/// A pixel buffer that can be written to, but not read from
	class RenderBuffer : public PixelBuffer {

		private:

			uint32_t rbo = 0;
			uint32_t w = 0;
			uint32_t h = 0;

			void framebuffer(GLenum attachment) const override;

		public:

			RenderBuffer();
			~RenderBuffer();

			void resize(int width, int height, GLenum internal_format, GLenum format) override;

			void use() const override;

			uint32_t width() const override;

			uint32_t height() const override;

	};

	/// Managed texture build from smaller regions called sprites
	class TileSet : public PixelBuffer {

		public:

			struct Ref {
				const int x;
				const int y;
			};

			/// Create reference for tile [x, y]
			static Ref of(int x, int y);

		private:

			Texture texture;
			int tw, th, line, column;

			void framebuffer(GLenum attachment) const override;

		public:

			TileSet(const char* path, uint32_t tile);

			TileSet(const char* path, uint32_t tw, uint32_t th);

			void resize(int w, int h, GLenum internal_format, GLenum format) override;

			void use() const override;

			/// Get current atlas width in pixels
			uint32_t width() const override;

			/// Get current atlas height in pixels
			uint32_t height() const override;

			/// Get the number of sprite columns in atlas
			uint32_t columns() const;

			/// Get the number of sprite rows in atlas
			uint32_t rows() const;

		public:

			/// Get given tile from atlas
			Sprite sprite(int x, int y);

			/// Get Nth sprite from atlas
			Sprite sprite(int index);

			/// Get sprite by sprite reference
			Sprite sprite(Ref ref);

	};

}
