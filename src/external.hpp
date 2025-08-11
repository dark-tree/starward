#pragma once

#define STB_VORBIS_HEADER_ONLY

// C
#include <cinttypes>

// GLM extensions
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

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
#include <filesystem>
#include <regex>

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

/// Can be used to mark pointers that can be null
#define NULLABLE

using Runnable = std::function<void()>;
