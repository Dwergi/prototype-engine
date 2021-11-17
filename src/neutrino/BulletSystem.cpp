//
// BulletSystem.cpp - A system to update BulletComponents.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#include "PCH.h"
#include "BulletSystem.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "ColourComponent.h"
#include "FPSCameraComponent.h"
#include "IAsyncHitTest.h"
#include "ICamera.h"
#include "Input.h"
#include "LightComponent.h"
#include "MeshComponent.h"
#include "MessageQueue.h"
#include "PlayerComponent.h"
#include "TransformComponent.h"

#include "ddc/ScratchEntity.h"

#include "ddm/HitTest.h"

#include "ddr/Mesh.h"

#include "neutrino/BulletComponent.h"

static dd::Service<dd::IAsyncHitTest> s_hitTest;
static dd::Service<ddc::MessageQueue> s_messageQueue;
static dd::Service<dd::Input> s_input;

namespace neut
{
	BulletSystem::BulletSystem() :
		ddc::System("Bullets")
	{
		m_speed = 50;
		m_colour = glm::vec3(1, 0, 0);
		m_intensity = 0.5f;
		m_scale = 0.1f;
		m_attenuation = 0.1f;

		RequireWrite<neut::BulletComponent>("bullets");
		RequireWrite<dd::TransformComponent>("bullets");

		RequireRead<dd::MeshComponent>("dynamic_meshes");
		RequireRead<dd::TransformComponent>("dynamic_meshes");
		OptionalRead<dd::BoundBoxComponent>("dynamic_meshes");
		OptionalRead<dd::BoundSphereComponent>("dynamic_meshes");
		RequireTag(ddc::Tag::Dynamic, "dynamic_meshes");

		RequireRead<dd::PlayerComponent>("player");
		RequireRead<dd::FPSCameraComponent>("player");

		m_bulletHitMessage = ddc::MessageType::Register<neut::BulletHitMessage>("BulletHit");
	}

	void BulletSystem::Initialize(ddc::EntityLayer& entities)
	{

	}

	void BulletSystem::FireBullet(ddc::UpdateData& update_data, const ddr::ICamera& camera)
	{
		ddc::ScratchEntity scratch = ddc::ScratchEntity::Create<neut::BulletComponent, dd::TransformComponent, dd::MeshComponent, dd::BoundSphereComponent, dd::BoundBoxComponent, dd::LightComponent, dd::ColourComponent>();
		scratch.AddTag(ddc::Tag::Visible);

		dd::TransformComponent* transform = scratch.Access<dd::TransformComponent>();
		transform->Scale = glm::vec3(m_scale);
		transform->Position = camera.GetPosition();
		transform->Update();

		neut::BulletComponent* bullet = scratch.Access<neut::BulletComponent>();
		bullet->Velocity = camera.GetDirection() * m_speed;

		dd::MeshComponent* mesh = scratch.Access<dd::MeshComponent>();

		mesh->Mesh = ddr::MeshHandle("sphere");

		dd::ColourComponent* colour = scratch.Access<dd::ColourComponent>();
		colour->Colour = glm::vec4(m_colour, 1);

		dd::BoundSphereComponent* bsphere = scratch.Access<dd::BoundSphereComponent>();
		bsphere->Sphere.Radius = 1.0f;

		dd::BoundBoxComponent* bbox = scratch.Access<dd::BoundBoxComponent>();
		bbox->BoundBox = mesh->Mesh.Get()->GetBoundBox();

		dd::LightComponent* light = scratch.Access<dd::LightComponent>();
		light->LightType = dd::LightType::Point;
		light->Ambient = 0;
		light->Intensity = m_intensity;
		light->Colour = m_colour;
		light->Attenuation = m_attenuation;

		update_data.CreateEntity(std::move(scratch));

		m_fireBullet = false;
	}

	void BulletSystem::KillBullet(ddc::UpdateData& update, ddc::Entity entity, neut::BulletComponent& bullet)
	{
		if (bullet.PendingHit.Valid)
		{
			s_hitTest->ReleaseResult(bullet.PendingHit);
		}

		if (bullet.HitResult.IsValid())
		{
			neut::BulletHitMessage payload;
			payload.Position = bullet.HitResult.Position();
			payload.Normal = bullet.HitResult.Normal();
			payload.Velocity = bullet.Velocity;
			payload.HitEntity = bullet.HitResult.Entity();

			ddc::Message msg(m_bulletHitMessage);
			msg.SetPayload(payload);

			s_messageQueue->Send(msg);
		}

		update.DestroyEntity(entity);
	}

	bool BulletSystem::HitTestDynamicMeshes(neut::BulletComponent& bullet, dd::TransformComponent& bullet_transform, const ddc::UpdateBufferView& meshes, float delta_t, glm::vec3& out_pos)
	{
		auto mesh_cmps = meshes.Read<dd::MeshComponent>();
		auto mesh_transforms = meshes.Read<dd::TransformComponent>();
		auto mesh_bboxes = meshes.Read<dd::BoundBoxComponent>();
		auto mesh_bspheres = meshes.Read<dd::BoundSphereComponent>();

		glm::vec3 initial_pos = bullet_transform.Position;
		ddm::Ray ray(initial_pos, bullet.Velocity, glm::length(bullet.Velocity * delta_t));

		for (size_t i = 0; i < meshes.Size(); ++i)
		{
			float hit_distance;
			glm::vec3 hit_normal;

			if (ddm::HitTestMesh(ray, mesh_cmps[i], mesh_transforms[i], mesh_bspheres.Get(i), mesh_bboxes.Get(i), hit_distance, hit_normal))
			{
				bullet.HitResult = dd::HitResult(ray);
				bullet.HitResult.RegisterHit(hit_distance, hit_normal, meshes.Entities()[i]);

				bullet.Lifetime = bullet.Age;
				return true;
			}
		}

		return false;
	}

	void BulletSystem::Update(ddc::UpdateData& update)
	{
		if (s_input->GotInput(dd::InputAction::SHOOT))
		{
			const auto& player = update.Data("player");
			auto cameras = player.Read<dd::FPSCameraComponent>();

			FireBullet(update, cameras[0]);
		}

		const auto& dynamic_meshes = update.Data("dynamic_meshes");
		const auto& bullet_data = update.Data("bullets");

		auto bullets = bullet_data.Write<neut::BulletComponent>();
		auto bullet_transforms = bullet_data.Write<dd::TransformComponent>();

		float delta_t = update.Delta();

		m_count = (int) bullet_data.Size();

		for (size_t i = 0; i < bullet_data.Size(); ++i)
		{
			neut::BulletComponent& bullet = bullets[i];
			bullet.Age += delta_t;

			if (bullet.Age >= bullet.Lifetime)
			{
				KillBullet(update, bullet_data.Entities()[i], bullet);
				continue;
			}

			dd::TransformComponent& bullet_transform = bullet_transforms[i];

			// check dynamic meshes
			glm::vec3 initial_pos = bullet_transform.Position;
			glm::vec3 new_pos;
			if (!HitTestDynamicMeshes(bullet, bullet_transform, dynamic_meshes, delta_t, new_pos))
			{
				new_pos = initial_pos + bullet.Velocity * delta_t;
			}

			bullet_transform.Position = new_pos;
			bullet_transform.Update();

			// hit test for bullet hit location
			if (bullet.PendingHit.Completed)
			{
				continue;
			}

			if (!bullet.PendingHit.Valid)
			{
				ddm::Ray ray(initial_pos, bullet.Velocity, glm::length(bullet.Velocity * bullet.Lifetime));
				bullet.PendingHit = s_hitTest->ScheduleHitTest(ray);
			}
			else
			{
				dd::HitResult result;
				if (s_hitTest->FetchResult(bullet.PendingHit, result))
				{
					if (result.Distance() < FLT_MAX)
					{
						bullet.HitResult = result;

						float hit_lifetime = result.Distance() / glm::length(bullet.Velocity);
						bullet.Lifetime = ddm::min(bullet.Lifetime, hit_lifetime);
					}

					bullet.PendingHit.Completed = true;
				}
			}
		}
	}

	void BulletSystem::DrawDebugInternal()
	{
		ImGui::SetWindowSize(ImVec2(200, 300), ImGuiCond_FirstUseEver);

		ImGui::Value("Count", m_count);

		ImGui::DragFloat("Scale", &m_scale, 0.01f, 0, 10, "%.1f");
		ImGui::DragFloat("Speed", &m_speed, 1, 0, 1000, "%.0f");

		ImGui::ColorEdit3("Colour", glm::value_ptr(m_colour));
		ImGui::DragFloat("Intensity", &m_intensity, 0.01, 0, 20);
		ImGui::DragFloat("Attenuation", &m_attenuation, 0.001, 0.001, 1);
	}
}