//
// BulletSystem.cpp - A system to update BulletComponents.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#include "PCH.h"
#include "BulletSystem.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "BulletComponent.h"
#include "ColourComponent.h"
#include "FPSCameraComponent.h"
#include "HitTest.h"
#include "IAsyncHitTest.h"
#include "ICamera.h"
#include "InputBindings.h"
#include "LightComponent.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MessageQueue.h"
#include "PlayerComponent.h"
#include "TransformComponent.h"

namespace dd
{
	BulletSystem::BulletSystem( IAsyncHitTest& hit_test ) :
		ddc::System( "Bullets" ),
		m_hitTest( hit_test )
	{
		m_speed = 50;
		m_colour = glm::vec3( 1, 0, 0 );
		m_intensity = 0.5f;
		m_scale = 0.1f;
		m_attenuation = 0.1f;

		RequireWrite<dd::BulletComponent>( "bullets" );
		RequireWrite<dd::TransformComponent>( "bullets" );
		
		RequireRead<dd::MeshComponent>( "dynamic_meshes" );
		RequireRead<dd::TransformComponent>( "dynamic_meshes" );
		OptionalRead<dd::BoundBoxComponent>( "dynamic_meshes" );
		OptionalRead<dd::BoundSphereComponent>( "dynamic_meshes" );
		RequireTag( ddc::Tag::Dynamic, "dynamic_meshes" );

		RequireRead<dd::PlayerComponent>( "player" );
		RequireRead<dd::FPSCameraComponent>( "player" );
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
	
	void BulletSystem::FireBullet( ddc::World& world, const ddr::ICamera& camera )
	{
		ddc::Entity entity = world.CreateEntity<dd::BulletComponent, dd::TransformComponent, dd::MeshComponent, dd::BoundSphereComponent, dd::BoundBoxComponent, dd::LightComponent, dd::ColourComponent>();
		world.AddTag( entity, ddc::Tag::Visible );

		dd::TransformComponent* transform;
		world.Access( entity, transform );

		transform->Scale = glm::vec3( m_scale );
		transform->Position = camera.GetPosition();
		transform->Update();

		dd::BulletComponent* bullet;
		world.Access( entity, bullet );
		bullet->Velocity = camera.GetDirection() * m_speed;

		dd::MeshComponent* mesh;
		world.Access( entity, mesh );
		mesh->Mesh = ddr::MeshManager::Instance()->Find( "sphere" );

		dd::ColourComponent* colour;
		world.Access( entity, colour );
		colour->Colour = glm::vec4( m_colour, 1 );

		dd::BoundSphereComponent* bsphere;
		world.Access( entity, bsphere );
		bsphere->Sphere.Radius = 1.0f;

		dd::BoundBoxComponent* bbox;
		world.Access( entity, bbox );
		bbox->BoundBox = mesh->Mesh.Get()->GetBoundBox();

		dd::LightComponent* light;
		world.Access( entity, light );
		light->LightType = dd::LightType::Point;
		light->Ambient = 0;
		light->Intensity = m_intensity;
		light->Colour = m_colour;
		light->Attenuation = m_attenuation;

		m_fireBullet = false;
	}

	void BulletSystem::KillBullet( ddc::World& world, ddc::Entity entity, dd::BulletComponent& bullet )
	{
		if( bullet.PendingHit.Valid )
		{
			m_hitTest.ReleaseResult( bullet.PendingHit );
		}

		if( glm::length2( bullet.HitPosition ) > 0 )
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

		world.DestroyEntity( entity );
	}

	bool BulletSystem::HitTestDynamicMeshes( dd::BulletComponent& bullet, dd::TransformComponent& bullet_transform, const ddc::DataBuffer& meshes, float delta_t, glm::vec3& out_pos )
	{
		auto mesh_cmps = meshes.Read<dd::MeshComponent>();
		auto mesh_transforms = meshes.Read<dd::TransformComponent>();
		auto mesh_bboxes = meshes.Read<dd::BoundBoxComponent>();
		auto mesh_bspheres = meshes.Read<dd::BoundSphereComponent>();

		glm::vec3 initial_pos = bullet_transform.Position;
		dd::Ray ray( initial_pos, bullet.Velocity, glm::length( bullet.Velocity * delta_t ) );

		for( size_t i = 0; i < meshes.Size(); ++i )
		{
			float hit_distance;
			glm::vec3 hit_normal;

			if( ddm::HitTestMesh( ray, mesh_cmps[i], mesh_transforms[i], mesh_bspheres.Get( i ), mesh_bboxes.Get( i ), hit_distance, hit_normal ) )
			{
				bullet.HitPosition = initial_pos + glm::normalize( bullet.Velocity ) * hit_distance;
				bullet.HitNormal = hit_normal;
				DD_ASSERT( glm::length2( bullet.HitNormal ) > 0.01f );

				bullet.Lifetime = bullet.Age;
				return true;
			}
		}

		return false;
	}

	void BulletSystem::Update( const ddc::UpdateData& update )
	{
		ddc::World& world = update.World();

		if( m_fireBullet )
		{
			auto player = update.Data( "player" );
			auto cameras = player.Read<dd::FPSCameraComponent>();

			FireBullet( world, cameras[0] );
		}

		const ddc::DataBuffer& dynamic_meshes = update.Data( "dynamic_meshes" );
		const ddc::DataBuffer& bullet_data = update.Data( "bullets" );

		auto bullets = bullet_data.Write<dd::BulletComponent>();
		auto bullet_transforms = bullet_data.Write<dd::TransformComponent>();

		float delta_t = update.Delta();

		m_count = (int) bullet_data.Size();

		for( size_t i = 0; i < bullet_data.Size(); ++i )
		{
			dd::BulletComponent& bullet = bullets[ i ];
			bullet.Age += delta_t;

			if( bullet.Age >= bullet.Lifetime )
			{
				KillBullet( world, bullet_data.Entities()[i], bullet );
				continue;
			}

			dd::TransformComponent& bullet_transform = bullet_transforms[i];

			// check dynamic meshes
			glm::vec3 initial_pos = bullet_transform.Position;
			glm::vec3 new_pos;
			if( !HitTestDynamicMeshes( bullet, bullet_transform, dynamic_meshes, delta_t, new_pos ) )
			{
				new_pos = initial_pos + bullet.Velocity * delta_t;
			}

			bullet_transform.Position = new_pos;
			bullet_transform.Update();

			// hit test for bullet hit location
			if( bullet.PendingHit.Completed )
			{
				continue;
			}

			if( !bullet.PendingHit.Valid )
			{
				dd::Ray ray( initial_pos, bullet.Velocity, glm::length( bullet.Velocity * bullet.Lifetime ) );
				bullet.PendingHit = m_hitTest.ScheduleHitTest( ray );
			}
			else
			{
				HitResult result;
				if( m_hitTest.FetchResult( bullet.PendingHit, result ) )
				{
					if( result.Distance() < FLT_MAX )
					{
						bullet.HitPosition = result.Position();
						bullet.HitNormal = result.Normal();
						DD_ASSERT( glm::length2( bullet.HitNormal ) > 0.01f );

						float hit_lifetime = result.Distance() / glm::length( bullet.Velocity );
						bullet.Lifetime = ddm::min( bullet.Lifetime, hit_lifetime );
					}

					bullet.PendingHit.Completed = true;
				}
			}
		}
	}

	void BulletSystem::DrawDebugInternal( ddc::World& world )
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