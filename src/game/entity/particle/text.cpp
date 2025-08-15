#include "text.hpp"

#include "game/emitter.hpp"
#include "game/level/level.hpp"

TextEntity::TextEntity(double x, double y, std::string text, int lifetime)
	: Entity(32, x, y), text(text), lifetime(lifetime) {
}

bool TextEntity::shouldCollide(Entity* entity) {
	return false;
}

void TextEntity::tick(Level& level) {
	Entity::tick(level);
	y += 1.8;

	if (lifetime < age) {
		alpha -= 8;

		if (alpha <= 0) {
			dead = true;
		}
	}
}

void TextEntity::draw(Level& level, Renderer& renderer) {
	emitTextQuads(renderer.text, x, y + level.getScroll(), 16, 12, 255, 255, 0, alpha, text, TextMode::CENTER);
}
