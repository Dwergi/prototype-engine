//
// BulletSystem.cpp - A system to update BulletComponents.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#include "PCH.h"
#include "BulletSystem.h"

#include "BoundBoxComponent.h"
#include "BulletComponent.h"
#include "ColourComponent.h"
#include "IAsyncHitTest.h"
#include "ICamera.h"
#include "InputBindings.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MessageQueue.h"
#include "LightComponent.h"
#include "TransformComponent.h"

namespace dd
{
	BulletSystem::BulletSystem( ddr::ICamera& camera, IAsyncHitTest& hit_test ) :
		ddc::System( "Bullets" ),
		m_camera( camera ),
		m_hitTest( hit_test )
	{
		m_speed = 50;
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
		ddc::Entity entity = world.CreateEntity<dd::BulletComponent, dd::TransformComponent, dd::MeshComponent, dd::BoundBoxComponent, dd::LightComponent, dd::ColourComponent>();
		world.AddTag( entity, ddc::Tag::Visible );

		dd::TransformComponent* transform;
		world.Access( entity, transform );

		transform->Transform = glm::scale( glm::vec3( m_scale ) );
		transform->SetPosition( m_camera.GetPosition() );

		dd::BulletComponent* bullet;
		world.Access( entity, bullet );
		bullet->Velocity = m_camera.GetDirection() * m_speed;

		dd::MeshComponent* mesh;
		world.Access( entity, mesh );
		mesh->Mesh = ddr::Mesh::Find( "sphere" );

		dd::ColourComponent* colour;
		world.Access( entity, colour );
		colour->Colour = glm::vec4( m_colour, 1 );

		dd::BoundBoxComponent* bounds;
		world.Access( entity, bounds );
		bounds->BoundBox = ddr::Mesh::Get( mesh->Mesh )->GetBoundBox();

		dd::LightComponent* light;
		world.Access( entity, light );
		light->LightType = dd::LightType::Point;
		light->Ambient = 0;
		light->Intensity = m_intensity;
		light->Colour = m_colour;
		light->Attenuation = m_attenuation;

		m_fireBullet = false;
	}

	void BulletSystem::Update( const ddc::UpdateData& update )
	{
		ddc::World& world = update.World();

		if( m_fireBullet )
		{
			FireBullet( world );
		}

		const ddc::DataBuffer& data = update.Data();

		auto bullets = data.Write<dd::BulletComponent>();
		auto transforms = data.Write<dd::TransformComponent>();

		float delta_t = update.Delta();

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

				if( bullet.HitPosition != glm::vec3( 0 ) )
				{
					dd::BulletHitMessage payload;
					payload.Position = bullet.HitPosition;
					payload.SurfaceNormal = bullet.HitNormal;
					payload.Velocity = bullet.Velocity;

					dd::Message msg;
					msg.Type = dd::MessageType::BulletHit;
					msg.SetPayload( payload );

					world.Messages().Send( msg );
				}

				world.DestroyEntity( data.Entities()[i] );
				continue;
			}

			glm::vec3 start_pos = transforms[ i ].GetPosition();
			glm::vec3 end_pos = start_pos + bullet.Velocity * delta_t;
			
			transforms[ i ].SetPosition( end_pos );

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
					bullet.HitNormal = result.Normal();

					float hit_lifetime = result.Distance() / glm::length( bullet.Velocity );
					bullet.Lifetime = ddm::min( bullet.Lifetime, hit_lifetime );

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
		ImGui::DragFloat( "Speed", &m_speed, 1, 0, 1000, "%.0f" );

		ImGui::ColorEdit3( "Colour", glm::value_ptr( m_colour ) );
		ImGui::DragFloat( "Intensity", &m_intensity, 0.01, 0, 20 );
		ImGui::DragFloat( "Attenuation", &m_attenuation, 0.001, 0.001, 1 );
	}
}