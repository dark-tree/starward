#pragma once

#include <external.hpp>
#include <vector>
#include "rigidbody.hpp"
#include <chrono>

class Physics {
public:
	static void tick();
	static void add_rigidbody(RigidBody* rb);
	static void add_collider(Collider* c);
	static float get_world_scale();
	static float get_deltatime();

private:
	static std::vector<std::pair<Collider*, RigidBody*>> colliders_and_rigidbodies;
	static glm::vec2 gravity;
	static float world_scale;
	static std::chrono::high_resolution_clock::time_point previous_time;
	static float delta_time;
	static void update_deltatime();
};

