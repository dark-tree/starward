
#include "dust.hpp"

/*
 * DustEntity
 */

DustEntity::DustEntity(double x, double y, float fx, float fy, float angle, float rotation, float jitter, int lifetime, Color color)
: Entity(4, x, y), lifetime(lifetime), fx(fx), fy(fy), scalar(jitter), color(color) {

	this->angle = angle;
	this->fx += randomFloat(-1, 1) * jitter;
	this->fy += randomFloat(-1, 1) * jitter;
	this->rotation = randomFloat(-1, 1) * rotation;
}

bool DustEntity::shouldCollide(Entity* entity) {
	return false;
}

void DustEntity::tick(Level& level) {

	this->x += fx;
	this->y += fy;
	this->angle += rotation;

	if (age > lifetime) {
		dead = true;
	}

	age ++;
}

void DustEntity::draw(Level& level, TileSet& tileset, BufferWriter<Vert4f4b>& writer) {
	float delta = age / (float) lifetime;

	float radius = size + 5 * delta;
	float alpha = 255 - (delta * 200);

	emitEntityQuad(level, writer, tileset.sprite(0, 0), radius, angle, color.withAlpha(alpha));
}
