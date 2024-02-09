#include "rigidbody.hpp"
#include "physics.hpp"
#include "world.hpp"
#include <iostream>

RigidBody::RigidBody(Collider* collider, Transform* transform) : collider(collider), transform(transform) {
	Physics::add_rigidbody(this);
}

MapCollider::MapCollider(World* world) : Collider(), world(world) {
	Physics::add_collider(this);
}

bool BoxCollider::collides(Collider* other) {

	// world position
	glm::vec2 wp = glm::vec2(x, y);
	if (transform != nullptr) {
		wp += transform->position;
	}

	if (BoxCollider* box = dynamic_cast<BoxCollider*>(other); box != nullptr) {
		bool xOverlap = (wp.x <= box->x + box->w + box->transform->position.x) && (wp.x + w >= box->x + box->transform->position.x);
		bool yOverlap = (wp.y <= box->y + box->h + box->transform->position.y) && (wp.y + h >= box->y + box->transform->position.y);

		this->collision |= xOverlap && yOverlap;
		return xOverlap && yOverlap;
	}
	else if (MapCollider* map = dynamic_cast<MapCollider*>(other); map != nullptr) {
		World* world = map->getWorld();

		int tx = wp.x / 64;
		int ty = wp.y / 64;
		int txw = (wp.x + w) / 64;
		int tyh = (wp.y + h) / 64;

		if (tx < 0 || ty < 0 || txw >= world->width || tyh >= world->height) {
			return false;
		}

		bool leftbottom_hit = world->get(tx, ty) != 0;
		bool rightbottom_hit = world->get(txw, ty) != 0;
		bool lefttop_hit = world->get(tx, tyh) != 0;
		bool righttop_hit = world->get(txw, tyh) != 0;

		bool result = leftbottom_hit || rightbottom_hit || lefttop_hit || righttop_hit;

		this->collision |= result;
		return result;
	}
	return false;
}
