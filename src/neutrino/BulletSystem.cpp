//
// BulletSystem.cpp - A system to update BulletComponents.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#include "PCH.h"
#include "BulletSystem.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "neutrino/BulletComponent.h"
#include "ColourComponent.h"
#include "FPSCameraComponent.h"
#include "HitTest.h"
#include "IAsyncHitTest.h"
#include "ICamera.h"
#include "Input.h"
#include "LightComponent.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MessageQueue.h"
#include "PlayerComponent.h"
#include "Services.h"
#include "TransformComponent.h"

static dd::Service<dd::IAsyncHitTest> s_hitTest;
static dd::Service<ddc::MessageQueue> s_messageQueue;
static dd::Service<dd::Input> s_input;

namespace neut
{
	BulletSystem::BulletSystem() :
		ddc::System( "Bullets" )
	{
		m_speed = 50;
		m_colour = glm::vec3( 1, 0, 0 );
		m_intensity = 0.5f;
		m_scale = 0.1f;
		m_attenuation = 0.1f;

		RequireWrite<neut::BulletComponent>( "bullets" );
		RequireWrite<dd::TransformComponent>( "bullets" );
		
		RequireRead<dd::MeshComponent>( "dynamic_meshes" );
		RequireRead<dd::TransformComponent>( "dynamic_meshes" );
		OptionalRead<dd::BoundBoxComponent>( "dynamic_meshes" );
		OptionalRead<dd::BoundSphereComponent>( "dynamic_meshes" );
		RequireTag( ddc::Tag::Dynamic, "dynamic_meshes" );

		RequireRead<dd::PlayerComponent>( "player" );
		RequireRead<dd::FPSCameraComponent>( "player" );
	}

	void BulletSystem::Initialize( ddc::EntityLayer& entities )
	{

	}
	
	void BulletSystem::FireBullet( ddc::EntityLayer& entities, const ddr::ICamera& camera )
	{
		ddc::Entity entity = entities.CreateEntity<neut::BulletComponent, dd::TransformComponent, dd::MeshComponent, dd::BoundSphereComponent, dd::BoundBoxComponent, dd::LightComponent, dd::ColourComponent>();
		entities.AddTag( entity, ddc::Tag::Visible );

		dd::TransformComponent* transform;
		entities.Access( entity, transform );

		transform->Scale = glm::vec3( m_scale );
		transform->Position = camera.GetPosition();
		transform->Update();

		neut::BulletComponent* bullet;
		entities.Access( entity, bullet );
		bullet->Velocity = camera.GetDirection() * m_speed;

		dd::MeshComponent* mesh;
		entities.Access( entity, mesh );

		mesh->Mesh = ddr::MeshHandle( "sphere" );

		dd::ColourComponent* colour;
		entities.Access( entity, colour );
		colour->Colour = glm::vec4( m_colour, 1 );

		dd::BoundSphereComponent* bsphere;
		entities.Access( entity, bsphere );
		bsphere->Sphere.Radius = 1.0f;

		dd::BoundBoxComponent* bbox;
		entities.Access( entity, bbox );
		bbox->BoundBox = mesh->Mesh.Get()->GetBoundBox();

		dd::LightComponent* light;
		entities.Access( entity, light );
		light->LightType = dd::LightType::Point;
		light->Ambient = 0;
		light->Intensity = m_intensity;
		light->Colour = m_colour;
		light->Attenuation = m_attenuation;

		m_fireBullet = false;
	}

	void BulletSystem::KillBullet( ddc::EntityLayer& layer, ddc::Entity entity, neut::BulletComponent& bullet )
	{
		if( bullet.PendingHit.Valid )
		{
			s_hitTest->ReleaseResult( bullet.PendingHit );
		}

		if( glm::length2( bullet.HitPosition ) > 0 )
		{
			neut::BulletHitMessage payload;
			payload.Position = bullet.HitPosition;
			payload.SurfaceNormal = bullet.HitNormal;
			payload.Velocity = bullet.Velocity;

			ddc::Message msg;
			msg.Type = ddc::MessageType::BulletHit;
			msg.SetPayload( payload );

			s_messageQueue->Send( msg );
		}

		layer.DestroyEntity( entity );
	}

	bool BulletSystem::HitTestDynamicMeshes( neut::BulletComponent& bullet, dd::TransformComponent& bullet_transform, const ddc::UpdateDataBuffer& meshes, float delta_t, glm::vec3& out_pos )
	{
		auto mesh_cmps = meshes.Read<dd::MeshComponent>();
		auto mesh_transforms = meshes.Read<dd::TransformComponent>();
		auto mesh_bboxes = meshes.Read<dd::BoundBoxComponent>();
		auto mesh_bspheres = meshes.Read<dd::BoundSphereComponent>();

		glm::vec3 initial_pos = bullet_transform.Position;
		ddm::Ray ray( initial_pos, bullet.Velocity, glm::length( bullet.Velocity * delta_t ) );

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
		ddc::EntityLayer& entities = update.EntityLayer();
		
		if(s_input->GotInput(dd::InputAction::SHOOT))
		{
			const auto& player = update.Data( "player" );
			auto cameras = player.Read<dd::FPSCameraComponent>();

			FireBullet( entities, cameras[0] );
		}

		const auto& dynamic_meshes = update.Data( "dynamic_meshes" );
		const auto& bullet_data = update.Data( "bullets" );

		auto bullets = bullet_data.Write<neut::BulletComponent>();
		auto bullet_transforms = bullet_data.Write<dd::TransformComponent>();

		float delta_t = update.Delta();

		m_count = (int) bullet_data.Size();

		for( size_t i = 0; i < bullet_data.Size(); ++i )
		{
			neut::BulletComponent& bullet = bullets[ i ];
			bullet.Age += delta_t;

			if( bullet.Age >= bullet.Lifetime )
			{
				KillBullet( entities, bullet_data.Entities()[i], bullet );
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
				ddm::Ray ray( initial_pos, bullet.Velocity, glm::length( bullet.Velocity * bullet.Lifetime ) );
				bullet.PendingHit = s_hitTest->ScheduleHitTest( ray );
			}
			else
			{
				dd::HitResult result;
				if( s_hitTest->FetchResult( bullet.PendingHit, result ) )
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

	void BulletSystem::DrawDebugInternal()
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