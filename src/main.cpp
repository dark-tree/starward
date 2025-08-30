
#include <external.hpp>

#include "const.hpp"
#include "game/game.hpp"
#include "game/sounds.hpp"
#include "game/level/level.hpp"
#include "render/renderer.hpp"

// docs
// https://emscripten.org/docs/api_reference/html5.h.html

void checkViewport(float ratio, const std::function<void(int, int, int, int, glm::mat4& matrix)>& on_resize) {

	static int pw = 0;
	static int ph = 0;

	const auto [w, h] = getCanvasSize();

	if (w != pw || h != ph) {

		const float sx = 2.0f / w;
		const float sy = 2.0f / h;

		float rw = 0;
		float rh = 0;

		if (w > h * ratio) {
			rh = h;
			rw = h * ratio;
		} else {
			rh = w / ratio;
			rw = w;
		}

		// margin
		const float mx = (w - rw) / 2.0f;
		const float my = (h - rh) / 2.0f;

		// offset
		const float ox = sx * mx - 1;
		const float oy = sy * my - 1;

		// factor
		const float fx = sx * (rw / SW);
		const float fy = sy * (rh / SH);

		glm::mat4 matrix {
			fx * SW,   0,   0,   0,
			 0,  fy * SH,   0,   0,
			 0,   0,   1,   0,
			ox,  oy,   0,   1,
		};

		pw = w;
		ph = h;

		int rwi = rw;
		int rhi = rh;

		// resolution needs to be divisible by 2, not gonna lie, i don't know why
		if (rwi % 2 == 1) rwi ++;
		if (rhi % 2 == 1) rhi ++;

		on_resize(w, h, rwi, rhi, matrix);
		printf("Screen resized to %dx%d (with region: [%d, %d], offset: [%f, %f], factor: [%f, %f])\n", w, h, rwi, rhi, ox, oy, fx * SW, fy * SH);
	}

}

int main() {

	auto begin_time = std::chrono::steady_clock::now();

	Renderer renderer {};

	SoundSystem& system = SoundSystem::getInstance();
	Sounds::load();

	system.listener().gain(0.3f);

	Game game {};
	printf("All game systems ready!\n");
	printf("You can press and hold the TAB key to see credits & attribution.\n");
	platform_ready();

	int vw, vh;

	setMainLoop([&] {

		game.tick();

		// takes care of the screen ratio, calls the callback when the screen resizes
		checkViewport(ASPECT_RATIO, [&] (int w, int h, int rw, int rh, glm::mat4& matrix) {
			glm::mat4 static_matrix {
				 2.0f/SW, 0,       0,   0,
				 0,       2.0f/SH, 0,   0,
				 0,       0,       1,   0,
				-1,      -1,       0,   1,
			};

			renderer.level_shader.use();
			glUniformMatrix4fv(renderer.level_shader.uniform("uMatrix"), 1, GL_FALSE, glm::value_ptr(static_matrix));

			renderer.degrade_shader.use();
			glUniform2f(renderer.degrade_shader.uniform("uResolution"), rw, rh);
			glUniformMatrix4fv(renderer.degrade_shader.uniform("uMatrix"), 1, GL_FALSE, glm::value_ptr(matrix));

			vw = w;
			vh = h;
		});

		// video
		game.level->draw(renderer);
		renderer.done(vw, vh, game.level->getLinearAliveness(), begin_time);

		SoundSystem::getInstance().update();
		Input::clear();

	});

	printf("Main returned without error\n");
    return EXIT_SUCCESS;
}
