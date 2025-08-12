#include "ray.hpp"

#include "tesla.hpp"
#include "game/emitter.hpp"
#include "game/level/level.hpp"
#include "game/entity/player.hpp"

/*
 * RayBeamEntity
 */

RayBeamEntity::RayBeamEntity(const std::shared_ptr<TeslaAlienEntity>& left, const std::shared_ptr<TeslaAlienEntity>& right)
	: AlienEntity(left->x, left->y, 0), left(left), right(right) {
	this->rx = right->x;
	this->ry = right->y;
}

bool RayBeamEntity::checkPlacement(Level& level) {
	// if this was to return false only the beam would
	// be missing, the towers would still be generated
	return true;
}

void RayBeamEntity::onDamage(Level& level, int damage, Entity* damager) {
	// the ray entity cannot be damaged, ignore all attacks
}

void RayBeamEntity::tick(Level& level) {
	AlienEntity::tick(level);

	Collision collision = level.checkEntityCollision(this);

	if (collision.type == Collision::ENTITY) {
		if (auto* player = dynamic_cast<PlayerEntity*>(collision.entity)) {
			player->onDamage(level, 20, this);
		}
	}

	// this is mostly just a hack so that we have some position that makes sense
	// for unloading reasons, it should be named (lx, ly) not (x, y)
	this->x = left->x;
	this->y = left->y;

	this->rx = right->x;
	this->ry = right->y;

	this->power = std::max(0.2f, std::min(left->getHealth(), right->getHealth()) / 3.0f);
	this->collider = Box {20, -10, rx - x - 40, 20};

	if (left->isDead() || right->isDead()) {
		dead = true;
	}
}

void RayBeamEntity::drawElectricArc(BufferWriter<Vert4f4b>& writer, float scroll, int sx, int ex, int ey, float amplitude, float phase, float speed, float roughness, const Sprite& sprite, Color color) {

	for (int ox = sx; ox < ex; ox ++) {

		// we don't want much movement near the ends, this has maximal value at the center
		float strength = amplitude * power * std::min(
			static_cast<float>(std::abs(ox - sx)),
			static_cast<float>(std::abs(ex - ox))
		);

		float factor = 10;
		float slope = 0.5;

		float sigmoidal = factor / (1 + std::pow(M_E, - slope * strength)) - factor / 2;

		int variance = sigmoidal * glm::perlin(glm::vec2 {ox * roughness + age * speed, phase}) + ey;
		emitTileQuad(writer, sprite, ox, variance, 0.0f, scroll, color.r, color.g, color.b, color.a);
	}
}

void RayBeamEntity::draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) {

	glm::ivec2 start = level.toTilePos(x, y);
	glm::ivec2 end = level.toTilePos(rx, ry);

	float scroll = level.getScroll();
	int baseline = start.y;
	const Sprite& sprite = tileset.sprite(0, 0);

	// pure red doesn't look that good on it
	// so we cheat a bit and make it a bluish
	Color base = Color::red();
	base.g += 10;
	base.b += 30 * (glm::perlin(glm::vec2 {age * 0.01f, 100.0f}) + 1);

	drawElectricArc(writer, scroll, start.x, end.x, baseline, 0.7f, 3.0f, 0.2f, 0.45f, sprite, base.withAlpha(250));

	drawElectricArc(writer, scroll, start.x, end.x, baseline, 0.8f, 1.0f, 0.1f, 0.1f, sprite, base.withAlpha(120));
	drawElectricArc(writer, scroll, start.x, end.x, baseline, 0.8f, 77.0f, 0.07f, 0.1f, sprite, base.withAlpha(120));

	emitTileQuad(writer, tileset.sprite(0, 0), start.x, start.y, 0, scroll, 255, 50, 50, 255);
	emitTileQuad(writer, tileset.sprite(0, 0), end.x, start.y, 0, scroll, 255, 50, 50, 255);

}
