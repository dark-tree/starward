#pragma once
#include "alien.hpp"
#include "tesla.hpp"

class RayBeamEntity : public AlienEntity {

	private:

		float rx;
		float ry;
		float power;

		std::shared_ptr<TeslaAlienEntity> left, right;

		void drawElectricArc(RenderLayer& layer, float scroll, int sx, int ex, int ey, float amplitude, float phase, float speed, float roughness, Color color);

	public:

		RayBeamEntity(const std::shared_ptr<TeslaAlienEntity>& left, const std::shared_ptr<TeslaAlienEntity>& right);

		bool checkPlacement(Level& level) override;

		void onDamage(Level& level, int damage, Entity* damager) override;
		void tick(Level& level) override;
		void draw(Level& level, Renderer& renderer) override;

};
