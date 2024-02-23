#pragma once

#include <external.hpp>

class Transform {
public:
	Transform(const glm::vec2& position = { 0.0f, 0.0f }, float rotation = 0, const glm::vec2& scale = { 1.0f, 1.0f }) : position(position), rotation(rotation), scale(scale) {	}

	glm::vec2 position;
	float rotation;
	glm::vec2 scale;
};

class Physics;

class Collider {
public:
	virtual ~Collider() = default;
	
	bool get_collision() {
		return collision.x || collision.y;
	}

	bool get_collision_x() {
		return collision.x;
	}

	bool get_collision_y() {
		return collision.y;
	}

	float get_bounciness() {
		return bounciness;
	}

	void set_bounciness(float bounciness) {
		this->bounciness = bounciness;
	}

	void set_friction(const glm::vec2& friction) {
		this->friction = friction;
	}

	void set_friction(float f){
		this->friction = {f, f};
	}

protected:
	virtual bool collides(Collider* other) {
		return false;
	}

	glm::bvec2 collision = { false, false };
	float bounciness = 0.0f;
	glm::vec2 friction = { 0.0f, 0.0f };

	friend class Physics;
};

class World;
class MapCollider : public Collider {
public:
	MapCollider(World* world);

	World* getWorld() {
		return world;
	}

private:
	World* world;
};

class BoxCollider : public Collider {
public:
	BoxCollider(float x, float y, float w, float h, Transform* parent = nullptr) : Collider(), x(x), y(y), w(w), h(h), transform(parent) { }

	bool collides(Collider* other) override;

	~BoxCollider() override { }

private:
	float x, y, w, h;
	Transform* transform;
};


class RigidBody {
public:
	RigidBody(Collider* collider, Transform* transform);

	void apply_force(const glm::vec2& force) {
		velocity += force * mass;
	}

	glm::vec2& get_velocity() {
		return this->velocity;
	}

private:
	Collider* collider;
	Transform* transform;
	float mass = 1.0f;
	glm::vec2 velocity = { 0.0f, 0.0f };

	friend class Physics;
};
