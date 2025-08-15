#pragma once
#include <string>
#include "game/color.hpp"
#include "game/entity/entity.hpp"

class TextEntity : public Entity {

	private:

		int alpha = 255;
		std::string text;
		const int lifetime;

	public:

		TextEntity(double x, double y, std::string text, int lifetime);

		bool shouldCollide(Entity* entity) override;
		void tick(Level& level) override;
		void draw(Level& level, Renderer& renderer) override;

};
