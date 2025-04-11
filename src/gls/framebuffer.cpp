
#include "framebuffer.hpp"

gls::Framebuffer::Framebuffer() {
	glGenFramebuffers(1, &fbo);
}

gls::Framebuffer::Framebuffer(unsigned int fbo) {
	this->fbo = fbo;
}

gls::Framebuffer::~Framebuffer() {
	if (fbo != 0) {
		glDeleteFramebuffers(1, &fbo);
	}
}

void gls::Framebuffer::attach(const PixelBuffer& buffer, GLenum attachment) {
	use();
	buffer.framebuffer(attachment);
}

void gls::Framebuffer::use() const {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void gls::Framebuffer::clear(GLbitfield mask) const {
	use();
	glClear(mask);
}

const gls::Framebuffer& gls::Framebuffer::main() {
	static Framebuffer fb {0};
	return fb;
}