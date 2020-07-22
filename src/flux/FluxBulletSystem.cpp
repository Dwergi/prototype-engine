//
// FluxBulletSystem.cpp
// Copyright (C) Sebastian Nordgren 
// July 22nd 2020
//

#include "PCH.h"
#include "FluxBulletSystem.h"

#include "d2d/Transform2DComponent.h"

#include "ddm/AABB2D.h"

#include "flux/FluxBulletComponent.h"

namespace flux
{
	FluxBulletSystem::FluxBulletSystem(glm::vec2 map_size) :
		ddc::System("Bullets"),
		m_mapSize(map_size)
	{
		RequireWrite<d2d::Transform2DComponent>();
		RequireRead<flux::FluxBulletComponent>();
	}

	void FluxBulletSystem::Initialize(ddc::EntityLayer& layer)
	{

	}

	void FluxBulletSystem::Shutdown(ddc::EntityLayer& layer)
	{

	}

	DD_OPTIMIZE_OFF()

	void FluxBulletSystem::Update(const ddc::UpdateData& update_data)
	{
		const ddc::UpdateBufferView& update_buffer = update_data.Data();
		
		ddc::WriteView<d2d::Transform2DComponent> transforms = update_buffer.Write<d2d::Transform2DComponent>();
		ddc::ReadView<flux::FluxBulletComponent> bullets = update_buffer.Read<flux::FluxBulletComponent>();
		const dd::Span<ddc::Entity>& entities = update_buffer.Entities();

		float dt = update_data.Delta();
		size_t count = update_buffer.Size();

		for (int i = 0; i < update_buffer.Size(); ++i)
		{
			d2d::Transform2DComponent& transform_cmp = transforms[i];
			const flux::FluxBulletComponent& bullet_cmp = bullets[i];

			transform_cmp.Position += bullet_cmp.Velocity * dt;

			const glm::vec2 SLACK = { 1, 1 };
			const ddm::AABB2D SCREEN_BOUNDS(-SLACK, m_mapSize + SLACK);

			// out of bounds, kill the bullet
			if (!SCREEN_BOUNDS.Contains(transform_cmp.Position))
			{
				entities[i].Destroy();
			}
		}
	}
}