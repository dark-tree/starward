


namespace gls {

	inline void setViewportArea(int w, int h) {
		glViewport(0, 0, w, h);
	}

	inline void setScissorArea(int x, int y, int w, int h) {
		glScissor(x, y, w, h);
	}

	inline void scissor(bool enable) {
		if (enable) {
			glEnable(GL_SCISSOR_TEST);
		} else {
			glDisable(GL_SCISSOR_TEST);
		}
	}

	inline void blend(bool enable) {
		if (enable) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		} else {
			glDisable(GL_BLEND);
		}

	}

}