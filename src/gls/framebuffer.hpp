#pragma once

#include <external.hpp>
#include "texture.hpp"

class Framebuffer {

	private:

		unsigned int fbo;

	public:

		Framebuffer();
		Framebuffer(unsigned int fbo);
		~Framebuffer();

		Framebuffer(const Framebuffer& buffer) = delete;
		Framebuffer(Framebuffer&& buffer) = default;

		/// Add a pixel buffer to back this framebuffer
		void attach(const PixelBuffer& buffer, GLenum attachment);

		/// Bind this framebuffer
		void use() const;

		/// Clear this framebuffer
		void clear(GLbitfield mask = GL_COLOR_BUFFER_BIT) const;

	public:

		/// Get reference to the main screen framebuffer
		static const Framebuffer& main();

};
