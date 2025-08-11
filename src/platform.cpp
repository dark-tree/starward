
#include "external.hpp"
#include "platform.hpp"

#if !defined(__EMSCRIPTEN__) && defined(__linux__)
	int impl::screen_width;
	int impl::screen_height;
	PlatformKeyEventCallback impl::keydown_callback;
	PlatformKeyEventCallback impl::keyup_callback;
#endif
