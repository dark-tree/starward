
#include "framebuffer.hpp"

void Framebuffer::init() {
	glGenFramebuffers(1, &fbo);
}

void Framebuffer::init(int fbo) {
	this->fbo = fbo;
}

void Framebuffer::close() {
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
	static Framebuffer fb;
	fb.init(0);

	return fb;
}