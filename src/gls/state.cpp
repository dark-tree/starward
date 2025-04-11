
#include "state.hpp"

void gls::setViewportArea(int w, int h) {
	glViewport(0, 0, w, h);
}

void gls::setScissorArea(int x, int y, int w, int h) {
	glScissor(x, y, w, h);
}

void gls::setScissor(bool enable) {
	if (enable) {
		glEnable(GL_SCISSOR_TEST);
	} else {
		glDisable(GL_SCISSOR_TEST);
	}
}

void gls::setBlend(bool enable) {
	if (enable) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glDisable(GL_BLEND);
	}
}
