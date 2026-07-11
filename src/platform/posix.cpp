
/*
 * POSIX implementation of the platform abstraction interface
 */

#include "platform.hpp"

#if defined(__linux__)

	static int screen_width;
	static int screen_height;
	static PlatformKeyEventCallback keydown_callback;
	static PlatformKeyEventCallback keyup_callback;

	/// translates the system keycode into key enum
	static Key platform_translate_key(int key) {

		if (key == WXK_LEFT) return Key::LEFT;
		if (key == WXK_RIGHT) return Key::RIGHT;
		if (key == WXK_UP) return Key::UP;
		if (key == WXK_DOWN) return Key::DOWN;
		if (key == WXK_SPACE) return Key::SPACE;
		if (key == WXK_ESC) return Key::ESCAPE;
		if (key == WXK_TAB) return Key::TAB;
		if (key == WXK_ENTER) return Key::ENTER;
		if (key == 'W' || key == 'w') return Key::W;
		if (key == 'A' || key == 'a') return Key::A;
		if (key == 'D' || key == 'd') return Key::D;
		if (key == 'B' || key == 'b') return Key::B;

		return Key::UNDEF;
	}

	static void platform_close_handler() {
		exit(-1);
	}

	static void platform_keyboard_handler(int state, int keycode) {
		Key key = platform_translate_key(keycode);

		if (state == WINX_PRESSED) {
			keydown_callback(key);
		} else {
			keyup_callback(key);
		}
	}

	static void platform_resize_handler(int width, int height) {
		screen_width = width;
		screen_height = height;
	}

	/*
	 * Implementation
	 */

	void platform::write_string(std::string path, std::string data) {
		mkdir("./local", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

		std::ofstream f;
		f.open("./local/" + path, std::ofstream::binary | std::ifstream::out);
		f.write(data.data(), data.size());

		printf("Written '%s' into '/%s'\n", data.c_str(), path.c_str());
	}

	std::string platform::read_string(std::string path) {
		std::ifstream f;
		f.open("./local/" + path, std::ifstream::binary | std::ifstream::in);

		if (!f.is_open()) {
			return "";
		}

		f.seekg(0, std::ifstream::end);
		const auto size = f.tellg();
		f.seekg(0, std::ifstream::beg);

		std::string buffer;
		buffer.resize(size);
		f.read(buffer.data(), size);

		printf("Read '%s' from '/%s'\n", buffer.c_str(), path.c_str());

		return buffer;
	}

	std::string platform::get_startup_string() {
		return "";
	}

	[[noreturn]] inline void platform::exit(int code) {
		::exit(code);
	}

	void platform::set_keydown_callback(PlatformKeyEventCallback callback) {
		keydown_callback = callback;
	}

	void platform::set_keyup_callback(PlatformKeyEventCallback callback) {
		keyup_callback = callback;
	}

	void platform::get_render_target_size(int* width, int* height) {
		*width = screen_width;
		*height = screen_height;
	}

	[[noreturn]] void platform::set_main_loop(PlatformLoopCallback callback, int fps) {
		while (true) {
			callback();

			winxSwapBuffers();
			winxPollEvents();
		}
	}

	void platform::init() {
		printf("Began 'Winx/OpenGL' platform init...\n");

		winxHint(WINX_HINT_VSYNC, WINX_VSYNC_ENABLED);

		if (!winxOpen(GAME_NATIVE_WIDTH, GAME_NATIVE_HEIGHT, GAME_TITLE)) {
			printf("Failed to create WINX context! %s\n", winxGetError());
			platform::exit(-1);
		}

		// use GLAD to load OpenGL functions
		gladLoadGL();

		screen_width = GAME_NATIVE_WIDTH;
		screen_height = GAME_NATIVE_HEIGHT;

		winxSetCloseEventHandle(platform_close_handler);
		winxSetKeyboardEventHandle(platform_keyboard_handler);
		winxSetResizeEventHandle(platform_resize_handler);
	}

	void platform::ready() {

	}

#endif