//
// PhysicsSystem.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#include "PrecompiledHeader.h"
#include "PhysicsSystem.h"

#include "Mesh.h"
#include "MeshComponent.h"
#include "PhysicsComponent.h"
#include "Plane.h"
#include "TransformComponent.h"

DD_COMPONENT_CPP( dd::PhysicsComponent );

namespace dd
{
	PhysicsSystem::PhysicsSystem() :
		ddc::System( "Physics" )
	{
		m_gravity = glm::vec3( 0, -9.81, 0 );
		m_planeOrigin = glm::vec3( 0, 0, 0 );
		m_planeNormal = glm::vec3( 0, 1, 0 );

		RequireWrite<dd::TransformComponent>();
		RequireWrite<dd::PhysicsComponent>();
	}

	void PhysicsSystem::Initialize( ddc::World& world )
	{
		
	}

	void PhysicsSystem::Update( const ddc::UpdateData& data )
	{
		float delta_t = data.Delta();

		auto transforms = data.Write<dd::TransformComponent>();
		auto physics = data.Write<dd::PhysicsComponent>();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			dd::PhysicsComponent& phys = physics[ i ];
			phys.Acceleration = m_gravity;
			phys.Velocity += phys.Acceleration * delta_t;

			glm::vec3 pos = transforms[ i ].GetPosition();
			pos += phys.Velocity * delta_t;

			transforms[ i ].SetPosition( pos );

			Plane plane( m_planeOrigin, m_planeNormal );
			float overshoot = plane.DistanceTo( pos ) - phys.Sphere.Radius;
			if( overshoot < 0 )
			{
				// correct so that we're outside again
				pos += -overshoot * plane.Normal();
				phys.Velocity = glm::reflect( phys.Velocity, m_planeNormal ) * phys.Elasticity;
			}
		}
	}

	void PhysicsSystem::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::DragFloat3( "Gravity", glm::value_ptr( m_gravity ) );
		ImGui::DragFloat3( "Plane Origin", glm::value_ptr( m_planeOrigin ) );
		ImGui::DragFloat3( "Plane Normal", glm::value_ptr( m_planeNormal ) );
	}
}