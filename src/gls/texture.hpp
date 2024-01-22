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

			virtual void use() const = 0;
			virtual void resize(int w, int h, GLenum internal_format, GLenum format) const = 0;

	};

	class Texture : public PixelBuffer {

		protected:
		
			uint32_t tid;
		
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
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}

			~Texture() {
				glDeleteTextures(1, &tid);
			}

			void upload(unsigned char* data, int width, int height, int channels) {
				use();
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format_of(channels), GL_UNSIGNED_BYTE, data);
			}

			void resize(int w, int h, GLenum internal_format, GLenum format) const override {
				use();
				glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, GL_UNSIGNED_BYTE, nullptr);
			}
		
			void use() const override {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, tid);
			}

	};

	class RenderBuffer : public PixelBuffer {

		private:

			uint32_t rbo;

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

			void resize(int w, int h, GLenum internal_format, GLenum format) const override {
				use();
				glRenderbufferStorage(GL_RENDERBUFFER, internal_format, w, h);
			}

			void use() const override {
				glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			}

	};

}
