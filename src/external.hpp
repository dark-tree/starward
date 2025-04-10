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
#include <array>
#include <random>
#include <chrono>
#include <fstream>

// emscripten
#include <platform.hpp>

// libraries
#include <AL/al.h>
#include <AL/alc.h>
#include <stb_image.h>
#include <stb_vorbis.c>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util.hpp"

using Runnable = std::function<void()>;
