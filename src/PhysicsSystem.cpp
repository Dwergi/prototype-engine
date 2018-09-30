//
// PhysicsSystem.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#include "PCH.h"
#include "PhysicsSystem.h"

#include "Mesh.h"
#include "MeshComponent.h"
#include "PhysicsPlaneComponent.h"
#include "PhysicsSphereComponent.h"
#include "Plane.h"
#include "TransformComponent.h"

namespace dd
{
	PhysicsSystem::PhysicsSystem() :
		ddc::System( "Physics" )
	{
		m_gravity = glm::vec3( 0, -9.81, 0 );

		RequireWrite<dd::TransformComponent>( "dynamic_spheres" );
		RequireWrite<dd::PhysicsSphereComponent>( "dynamic_spheres" );
		RequireTag( ddc::Tag::Dynamic, "dynamic_spheres" );

		RequireRead<dd::TransformComponent>( "static_spheres" );
		RequireRead<dd::PhysicsSphereComponent>( "static_spheres" );
		RequireTag( ddc::Tag::Static, "static_spheres" );

		RequireRead<dd::TransformComponent>( "planes" );
		RequireRead<dd::PhysicsPlaneComponent>( "planes" );
		RequireTag( ddc::Tag::Static, "planes" );
	}

	void PhysicsSystem::Initialize( ddc::World& world )
	{
		
	}

	void PhysicsSystem::Update( const ddc::UpdateData& update )
	{
		float delta_t = update.Delta();

		const ddc::DataBuffer& dynamic_spheres = update.Data( "dynamic_spheres" );

		auto dynamic_spheres_transforms = dynamic_spheres.Write<dd::TransformComponent>();
		auto dynamic_spheres_physics = dynamic_spheres.Write<dd::PhysicsSphereComponent>();

		const ddc::DataBuffer& static_spheres = update.Data( "static_spheres" );

		auto static_spheres_transforms = static_spheres.Read<dd::TransformComponent>();
		auto static_spheres_physics = static_spheres.Read<dd::PhysicsSphereComponent>();

		const ddc::DataBuffer& planes = update.Data( "planes" );
		
		auto planes_transforms = planes.Read<dd::TransformComponent>();
		auto planes_physics = planes.Read<dd::PhysicsPlaneComponent>();

		for( size_t s = 0; s < dynamic_spheres.Size(); ++s )
		{
			dd::PhysicsSphereComponent& phys = dynamic_spheres_physics[s];
			
			if( phys.Resting )
				continue;

			phys.Acceleration = m_gravity;
			
			glm::vec3 velocity = phys.Velocity + phys.Acceleration * delta_t;
			float speed = glm::length( velocity );
			glm::vec3 normalized_velocity = glm::normalize( velocity );

			dd::TransformComponent& transform = dynamic_spheres_transforms[s];

			glm::vec3 pos = transform.GetPosition();
			pos += phys.Velocity * delta_t;

			bool contacting = false;

			for( size_t p = 0; p < planes.Size(); ++p )
			{
				const dd::PhysicsPlaneComponent& phys_plane = planes_physics[p];
				dd::Plane plane = phys_plane.Plane.GetTransformed( planes_transforms[p].Transform );

				float penetration = plane.DistanceTo( pos ) - phys.Sphere.Radius;
				if( penetration < 0 )
				{
					glm::vec3 plane_normal = plane.Normal();

					// correct so that we're outside again
					pos += -penetration * plane_normal;
					velocity = glm::reflect( normalized_velocity, plane_normal );
					velocity *= speed * phys.Elasticity * phys_plane.Elasticity;

					contacting = true;
				}
			}

			for( size_t ss = 0; ss < static_spheres.Size(); ++ss )
			{
				const dd::PhysicsSphereComponent& static_sphere = static_spheres_physics[ss];
				
				glm::vec3 static_pos = static_sphere.Sphere.Centre + static_spheres_transforms[ss].GetPosition();
				
				glm::vec3 sphere_normal = pos - static_pos;
				float distance = glm::length( sphere_normal );
				sphere_normal /= distance;

				if( distance < (phys.Sphere.Radius + static_sphere.Sphere.Radius) )
				{
					pos = static_pos + sphere_normal * (phys.Sphere.Radius + static_sphere.Sphere.Radius);

					velocity = glm::reflect( normalized_velocity, sphere_normal );
					velocity *= speed * phys.Elasticity * static_sphere.Elasticity;

					contacting = true;
				}
			}

			if( contacting && glm::length2( phys.Velocity ) < 0.1f )
			{
				velocity = glm::vec3( 0 );
				phys.Resting = true;
			}

			phys.Velocity = velocity;

			transform.SetPosition( pos );
		}
	}

	void PhysicsSystem::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::DragFloat3( "Gravity", glm::value_ptr( m_gravity ) );
	}
}