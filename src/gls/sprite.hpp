#pragma once

#include <external.hpp>

namespace gls {

	class Sprite {

		public:

			float min_u, min_v, max_u, max_v;

		public:

			Sprite(float min_u, float min_v, float max_u, float max_v);

	};

}