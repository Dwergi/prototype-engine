//
// BulletSystem.cpp - A system to update BulletComponents.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#include "PrecompiledHeader.h"
#include "BulletSystem.h"

#include "BoundsComponent.h"
#include "BulletComponent.h"
#include "IAsyncHitTest.h"
#include "InputBindings.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "LightComponent.h"
#include "TransformComponent.h"

DD_COMPONENT_CPP( dd::BulletComponent );

namespace dd
{
	BulletSystem::BulletSystem( IAsyncHitTest& hit_test ) :
		ddc::System( "Bullets" ),
		m_hitTest( hit_test )
	{
		RequireWrite<dd::BulletComponent>();
		RequireWrite<dd::TransformComponent>();
	}

	void BulletSystem::HandleInput( InputAction action, InputType type )
	{
		if( action == InputAction::SHOOT && type == InputType::RELEASED )
		{
			m_fireBullet = true;
		}
	}

	void BulletSystem::BindActions( InputBindings& bindings )
	{
		bindings.RegisterHandler( InputAction::SHOOT, 
			[this]( InputAction action, InputType type ) { HandleInput( action, type ); } );
	}

	void BulletSystem::Initialize( ddc::World& world )
	{

	}

	void BulletSystem::Update( const ddc::UpdateData& data )
	{
		ddc::World& world = data.World();

		if( m_fireBullet )
		{
			ddc::Entity entity = world.CreateEntity<dd::BulletComponent, dd::TransformComponent, dd::MeshComponent, dd::BoundsComponent, ddr::LightComponent>();
			world.AddTag( entity, ddc::Tag::Visible );	

			dd::TransformComponent* transform;
			world.Access( entity, transform );

			transform->Local = glm::scale( glm::vec3( 0.1, 0.1, 0.1 ) );
			transform->SetLocalPosition( glm::vec3( 0, 30, 0 ) );

			dd::BulletComponent* bullet;
			world.Access( entity, bullet );
			bullet->Velocity = glm::vec3( 10, 0, 10 );

			dd::MeshComponent* mesh;
			world.Access( entity, mesh );
			mesh->Mesh = ddr::Mesh::Find( "unitcube" );
			mesh->Colour = glm::vec4( 1, 0, 0, 1 );

			dd::BoundsComponent* bounds;
			world.Access( entity, bounds );
			bounds->LocalBox = ddr::Mesh::Get( mesh->Mesh )->GetBoundBox();

			ddr::LightComponent* light;
			world.Access( entity, light );
			light->Ambient = 0;
			light->Intensity = 0.5;
			light->Colour = glm::vec3( 1, 0, 0 );

			m_fireBullet = false;
		}

		auto bullets = data.Write<dd::BulletComponent>();
		auto transforms = data.Write<dd::TransformComponent>();

		float delta_t = data.Delta();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			dd::BulletComponent& bullet = bullets[ i ];
			bullet.Age += delta_t;

			if( bullet.Age >= bullet.Lifetime )
			{
				world.DestroyEntity( data.Entities()[i] );
				continue;
			}

			glm::vec3 start_pos = transforms[ i ].GetLocalPosition();
			glm::vec3 end_pos = start_pos + bullet.Velocity * delta_t;
			
			transforms[ i ].SetLocalPosition( end_pos );

			// hit test for bullet hit location
			if( bullet.HitCompleted )
			{
				continue;
			}

			if( bullet.PendingHit == nullptr )
			{
				dd::Ray ray( start_pos, bullet.Velocity );
				bullet.PendingHit = &m_hitTest.ScheduleHitTest( ray, glm::length( bullet.Velocity * bullet.Lifetime ) );
			}

			if( bullet.PendingHit != nullptr &&
				bullet.PendingHit->IsCompleted() )
			{
				float hit_lifetime = bullet.PendingHit->Distance() / glm::length( bullet.Velocity );
				bullet.Lifetime = dd::min( bullet.Lifetime, hit_lifetime );

				bullet.HitCompleted = true;
				bullet.PendingHit = nullptr;
			}
		}
	}
}