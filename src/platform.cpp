
#include "external.hpp"
#include "platform.hpp"

#if !defined(__EMSCRIPTEN__) && defined(__linux__)
	int __impl::__screen_width;
	int __impl::__screen_height;
	PlatformKeyEventCallback __impl::__keydown_callback;
	PlatformKeyEventCallback __impl::__keyup_callback;
#endif
