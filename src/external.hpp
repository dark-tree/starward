#pragma once

#define STB_VORBIS_HEADER_ONLY

// C
#include <cinttypes>

// C++
#include <functional>
#include <memory>
#include <stdexcept>
#include <list>
#include <algorithm>

// emscripten
#include <platform.hpp>

// libraries
#include <AL/al.h>
#include <AL/alc.h>
#include <stb/stb_image.h>
#include <stb/stb_vorbis.c>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using Runnable = std::function<void()>;
