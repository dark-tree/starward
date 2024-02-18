#pragma once

#include <external.hpp>
#include <vector>
#include "rigidbody.hpp"

class Physics {
public:
	static void tick();
	static void add_rigidbody(RigidBody* rb);
	static void add_collider(Collider* c);
	static float get_world_scale();

private:
	static std::vector<std::pair<Collider*, RigidBody*>> colliders_and_rigidbodies;
	static glm::vec2 gravity;
	static float world_scale;
};

