
#include "framebuffer.hpp"

Framebuffer::Framebuffer() {
	glGenFramebuffers(1, &fbo);
}

Framebuffer::Framebuffer(unsigned int fbo) {
	this->fbo = fbo;
}

Framebuffer::~Framebuffer() {
	if (fbo != 0) {
		glDeleteFramebuffers(1, &fbo);
	}
}

void Framebuffer::attach(const PixelBuffer& buffer, GLenum attachment) {
	use();
	buffer.framebuffer(attachment);
}

void Framebuffer::use() const {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Framebuffer::clear(GLbitfield mask) const {
	use();
	glClear(mask);
}

const Framebuffer& Framebuffer::main() {
	static Framebuffer fb {0};
	return fb;
}