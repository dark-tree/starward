
#include "platform.hpp"

#if !defined(__EMSCRIPTEN__) && defined(__linux__)
	PlatformKeyEventCallback __impl::__keydown_callback;
	PlatformKeyEventCallback __impl::__keyup_callback;
#endif
