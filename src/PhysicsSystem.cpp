//
// PhysicsSystem.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#include "PrecompiledHeader.h"
#include "PhysicsSystem.h"

#include "Mesh.h"
#include "MeshComponent.h"
#include "PhysicsPlaneComponent.h"
#include "PhysicsSphereComponent.h"
#include "Plane.h"
#include "TransformComponent.h"

DD_COMPONENT_CPP( dd::PhysicsSphereComponent );
DD_COMPONENT_CPP( dd::PhysicsPlaneComponent );

namespace dd
{
	PhysicsSystem::PhysicsSystem() :
		ddc::System( "Physics" )
	{
		m_gravity = glm::vec3( 0, -9.81, 0 );

		RequireWrite<dd::TransformComponent>( "spheres" );
		RequireWrite<dd::PhysicsSphereComponent>( "spheres" );

		RequireRead<dd::TransformComponent>( "planes" );
		RequireRead<dd::PhysicsPlaneComponent>( "planes" );
	}

	void PhysicsSystem::Initialize( ddc::World& world )
	{
		
	}

	void PhysicsSystem::Update( const ddc::UpdateData& update )
	{
		float delta_t = update.Delta();

		const ddc::DataBuffer& spheres = update.Data( "spheres" );

		auto spheres_transforms = spheres.Write<dd::TransformComponent>();
		auto spheres_physics = spheres.Write<dd::PhysicsSphereComponent>();

		const ddc::DataBuffer& planes = update.Data( "planes" );
		
		auto planes_transforms = planes.Read<dd::TransformComponent>();
		auto planes_physics = planes.Read<dd::PhysicsPlaneComponent>();

		for( size_t s = 0; s < spheres.Size(); ++s )
		{
			dd::PhysicsSphereComponent& phys = spheres_physics[s];
			
			if( phys.Resting )
				continue;

			phys.Acceleration = m_gravity;
			phys.Velocity += phys.Acceleration * delta_t;

			dd::TransformComponent& transform = spheres_transforms[s];

			glm::vec3 pos = transform.GetPosition();
			pos += phys.Velocity * delta_t;

			for( size_t p = 0; p < planes.Size(); ++p )
			{
				const dd::PhysicsPlaneComponent& phys_plane = planes_physics[p];
				dd::Plane plane = phys_plane.Plane.GetTransformed( planes_transforms[p].Transform );

				float penetration = plane.DistanceTo( pos ) - phys.Sphere.Radius;
				if( penetration < 0 )
				{
					float speed = glm::length( phys.Velocity );

					// correct so that we're outside again
					pos += -penetration * plane.Normal();
					phys.Velocity = glm::reflect( glm::normalize( phys.Velocity ), plane.Normal() );
					phys.Velocity *= speed * phys.Elasticity * phys_plane.Elasticity;

					if( glm::length2( phys.Velocity ) < 0.01f )
					{
						phys.Velocity = glm::vec3( 0 );
						phys.Resting = true;
					}
				}
			}

			transform.SetPosition( pos );
		}
	}

	void PhysicsSystem::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::DragFloat3( "Gravity", glm::value_ptr( m_gravity ) );
	}
}