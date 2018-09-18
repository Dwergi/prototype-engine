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

#include "imgui/imgui.h"

DD_COMPONENT_CPP( dd::BulletComponent );

namespace dd
{
	BulletSystem::BulletSystem( IAsyncHitTest& hit_test ) :
		ddc::System( "Bullets" ),
		m_hitTest( hit_test )
	{
		m_direction = glm::vec3( 1, 0, 0 );
		m_speed = 20;
		m_origin = glm::vec3( 0, 30, 0 );
		m_colour = glm::vec3( 1, 0, 0 );
		m_intensity = 0.5f;
		m_scale = 0.1f;
		m_attenuation = 0.1f;

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
	
	void BulletSystem::FireBullet( ddc::World& world )
	{
		ddc::Entity entity = world.CreateEntity<dd::BulletComponent, dd::TransformComponent, dd::MeshComponent, dd::BoundsComponent, ddr::LightComponent>();
		world.AddTag( entity, ddc::Tag::Visible );

		dd::TransformComponent* transform;
		world.Access( entity, transform );

		transform->Local = glm::scale( glm::vec3( m_scale ) );
		transform->SetLocalPosition( m_origin );

		dd::BulletComponent* bullet;
		world.Access( entity, bullet );
		bullet->Velocity = glm::normalize( m_direction ) * m_speed;

		dd::MeshComponent* mesh;
		world.Access( entity, mesh );
		mesh->Mesh = ddr::Mesh::Find( "unitcube" );
		mesh->Colour = glm::vec4( m_colour, 1 );

		dd::BoundsComponent* bounds;
		world.Access( entity, bounds );
		bounds->LocalBox = ddr::Mesh::Get( mesh->Mesh )->GetBoundBox();

		ddr::LightComponent* light;
		world.Access( entity, light );
		light->Ambient = 0;
		light->Intensity = m_intensity;
		light->Colour = m_colour;
		light->Attenuation = m_attenuation;

		m_fireBullet = false;
	}

	void BulletSystem::Update( const ddc::UpdateData& data )
	{
		ddc::World& world = data.World();

		if( m_fireBullet )
		{
			FireBullet( world );
		}

		auto bullets = data.Write<dd::BulletComponent>();
		auto transforms = data.Write<dd::TransformComponent>();

		float delta_t = data.Delta();

		m_count = (int) data.Size();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			dd::BulletComponent& bullet = bullets[ i ];
			bullet.Age += delta_t;

			if( bullet.Age >= bullet.Lifetime )
			{
				if( bullet.PendingHit.Valid )
				{
					m_hitTest.ReleaseResult( bullet.PendingHit );
				}

				world.DestroyEntity( data.Entities()[i] );
				continue;
			}

			glm::vec3 start_pos = transforms[ i ].GetLocalPosition();
			glm::vec3 end_pos = start_pos + bullet.Velocity * delta_t;
			
			transforms[ i ].SetLocalPosition( end_pos );

			// hit test for bullet hit location
			if( bullet.PendingHit.Completed )
			{
				continue;
			}

			if( !bullet.PendingHit.Valid )
			{
				dd::Ray ray( start_pos, bullet.Velocity );
				bullet.PendingHit = m_hitTest.ScheduleHitTest( ray, glm::length( bullet.Velocity * bullet.Lifetime ) );
			}
			else
			{
				HitResult result;

				if( m_hitTest.FetchResult( bullet.PendingHit, result ) )
				{
					bullet.HitPosition = result.Position();

					float hit_lifetime = result.Distance() / glm::length( bullet.Velocity );
					bullet.Lifetime = dd::min( bullet.Lifetime, hit_lifetime );

					bullet.PendingHit.Completed = true;
				}
			}
		}
	}

	void BulletSystem::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::SetWindowSize( ImVec2( 200, 300 ), ImGuiCond_FirstUseEver );

		ImGui::Value( "Count", m_count );

		ImGui::DragFloat( "Scale", &m_scale, 0.01f, 0, 10, "%.1f" );
		ImGui::DragFloat3( "Origin", glm::value_ptr( m_origin ), 0.1, -100, 100, "%.1f" );
		ImGui::DragFloat3( "Direction", glm::value_ptr( m_direction ), 0.001, -1, 1, "%.3f" );
		ImGui::DragFloat( "Speed", &m_speed, 1, 0, 1000, "%.0f" );

		ImGui::ColorEdit3( "Colour", glm::value_ptr( m_colour ) );
		ImGui::DragFloat( "Intensity", &m_intensity, 0.01, 0, 20 );
		ImGui::DragFloat( "Attenuation", &m_attenuation, 0.001, 0.001, 1 );
	}
}