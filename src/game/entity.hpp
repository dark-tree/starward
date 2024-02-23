#pragma once

#include "world.hpp"
#include "physics.hpp"
#include <iostream>

int key_code = 0;

struct Entity {

	protected:

		Transform transform;

	public:

		virtual ~Entity() {};
		virtual void render(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) = 0;
		virtual void tick(World& world) = 0;

};


class Player : public Entity {

	protected:

		BoxCollider collider;
		RigidBody rigid_body;
		float coyote_timer = 0;
		int move_direction_x = 0;
		const float coyote_time = 0.2f;

	public:

		Player() : Entity(), collider(16, 0, 32, 32, &transform), rigid_body(&collider, &transform) {
			transform.position = { 100.0f, 100.0f };
			collider.set_friction({ 0.0f, 1.0f });
		}

		void render(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) override {
			gls::Sprite s = tileset.sprite(11);

			uint8_t v = collider.get_collision() ? 255 : 100;
			float x = transform.position.x;
			float y = transform.position.y;

			buffer.push({x + 0,   0 + y,  s.min_u, s.min_v, 255, 255, v, 255});
			buffer.push({x + 64,  0 + y,  s.max_u, s.min_v, 255, 255, v, 255});
			buffer.push({x + 64, 64 + y,  s.max_u, s.max_v, 255, 255, v, 255});
			buffer.push({x + 64, 64 + y,  s.max_u, s.max_v, 255, 255, v, 255});
			buffer.push({x + 0,  64 + y,  s.min_u, s.max_v, 255, 255, v, 255});
			buffer.push({x + 0,   0 + y,  s.min_u, s.min_v, 255, 255, v, 255});
		}

		void tick(World& world) override {
			float delta_time = Physics::get_deltatime();

			coyote_timer = std::max(0.0f, coyote_timer - delta_time);

			bool left = gls::Input::is_pressed(Key::LEFT);
			bool right = gls::Input::is_pressed(Key::RIGHT);

			if (left) {
				rigid_body.get_velocity().x = -3;
				if (collider.get_collision()) {
					coyote_timer = coyote_time;
				}
			}
			if (right) {
				rigid_body.get_velocity().x = 3;
				if (collider.get_collision()) {
					coyote_timer = coyote_time;
				}
			}

			// jump
			if (gls::Input::is_typed(Key::SPACE)) {
				bool on_ground = collider.get_collision_y();
				bool coyote = coyote_timer > 0.0f;
				// if is on groun or just left the ground/wall and is moving to the side and is not going up too fast
				if (on_ground || (coyote && !collider.get_collision_x() && (left || right) && rigid_body.get_velocity().y < 2.0f)) {
					rigid_body.get_velocity().y = 8;
					coyote_timer = 0.0f;
				}
			}
		}

};

class Box : public Entity {
protected:

	BoxCollider collider;
	RigidBody rigid_body;

public:

	Box(const glm::vec2& position) : Entity(), collider(16, 0, 32, 32, &transform), rigid_body(&collider, &transform) {
		transform.position = position;
	}

	void render(gls::TileSet& tileset, gls::BufferWriter<gls::Vert4f4b>& buffer) override {
		gls::Sprite s = tileset.sprite(11);

		uint8_t v = 255;
		float x = transform.position.x;
		float y = transform.position.y;

		buffer.push({ x + 0,   0 + y,  s.min_u, s.min_v, 255, 255, v, 255 });
		buffer.push({ x + 64,  0 + y,  s.max_u, s.min_v, 255, 255, v, 255 });
		buffer.push({ x + 64, 64 + y,  s.max_u, s.max_v, 255, 255, v, 255 });
		buffer.push({ x + 64, 64 + y,  s.max_u, s.max_v, 255, 255, v, 255 });
		buffer.push({ x + 0,  64 + y,  s.min_u, s.max_v, 255, 255, v, 255 });
		buffer.push({ x + 0,   0 + y,  s.min_u, s.min_v, 255, 255, v, 255 });
	}

	void tick(World& world) override {

	}
};
