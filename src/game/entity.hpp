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
		const float coyote_time = 0.2f;

	public:

		Player() : Entity(), collider(16, 0, 32, 32, &transform), rigid_body(&collider, &transform) {
			transform.position = { 100.0f, 100.0f };
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
			float delta_time = 1.0f / 60.0f;

			coyote_timer = std::max(0.0f, coyote_timer - delta_time);

			if (gls::Input::is_pressed(gls::Key::LEFT)) {
				rigid_body.get_velocity().x = -3;
				if (collider.get_collision()) {
					coyote_timer = coyote_time;
				}
			} 
			if (gls::Input::is_pressed(gls::Key::RIGHT)) {
				rigid_body.get_velocity().x = 3;
				if (collider.get_collision()) {
					coyote_timer = coyote_time;
				}
			}

			if (gls::Input::is_typed(gls::Key::SPACE) && (collider.get_collision() || coyote_timer > 0.0f)) {
				rigid_body.apply_force({0, 8});
				coyote_timer = 0.0f;
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
