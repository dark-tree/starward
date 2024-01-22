#pragma once

#include "texture.hpp"

namespace gls {

	class Framebuffer {

		private:

			unsigned int fbo;

		public:

			Framebuffer() {
				glGenFramebuffers(1, &fbo);
			}

			Framebuffer(unsigned int fbo) {
				this->fbo = fbo;
			}

			~Framebuffer() {
				if (fbo != 0) {
					glDeleteFramebuffers(1, &fbo);  
				}
			}

			void attach(const PixelBuffer& buffer, GLenum attachment) {
				use();
				buffer.framebuffer(attachment);
			}

			void use() const {
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			}

			void clear(GLbitfield mask = GL_COLOR_BUFFER_BIT) const {
				glClear(mask);
			}

		public:

			static const Framebuffer& main() {
				static Framebuffer fb {0};
				return fb;
			}

	};

}
