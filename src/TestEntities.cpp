#include "PCH.h"
#include "TestEntities.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "ColourComponent.h"
#include "InputKeyBindings.h"
#include "Input.h"
#include "LightComponent.h"
#include "MeshComponent.h"
#include "ParticleSystemComponent.h"
#include "PhysicsSphereComponent.h"
#include "PhysicsPlaneComponent.h"
#include "PlayerComponent.h"
#include "RayComponent.h"
#include "neutrino/ShipComponent.h"
#include "TransformComponent.h"

#include "ddc/EntityPrototype.h"

namespace dd
{
	static dd::Service<ddc::EntityPrototypeManager> s_entityProtoManager;

	ddc::Entity TestEntities::CreateMeshEntity(ddc::EntityLayer& entities, const ddr::MeshHandle& mesh_h, glm::vec4 colour, glm::vec3 pos, glm::quat rot, glm::vec3 scale)
	{
		ddc::Entity entity = entities.CreateEntity<dd::TransformComponent, dd::MeshComponent, dd::BoundBoxComponent, dd::ColourComponent>();

		dd::TransformComponent* transform_cmp = entities.Access<dd::TransformComponent>(entity);
		transform_cmp->Position = pos;
		transform_cmp->Scale = scale;
		transform_cmp->Rotation = rot;
		transform_cmp->Update();

		dd::MeshComponent* mesh_cmp = entities.Access<dd::MeshComponent>(entity);
		mesh_cmp->Mesh = mesh_h;

		dd::ColourComponent* colour_cmp = entities.Access<dd::ColourComponent>(entity);
		colour_cmp->Colour = colour;

		dd::BoundBoxComponent* bounds_cmp = entities.Access<dd::BoundBoxComponent>(entity);
		bounds_cmp->BoundBox = mesh_h.Get()->GetBoundBox();

		entities.AddTag(entity, ddc::Tag::Visible);

		return entity;
	}

	ddc::Entity TestEntities::CreateBall(ddc::EntityLayer& entities, glm::vec3 translation, glm::vec4 colour, float size)
	{
		ddr::MeshHandle mesh_h("sphere");

		ddc::Entity entity = CreateMeshEntity(entities, mesh_h, colour,
			translation,
			glm::angleAxis(glm::radians(45.0f), glm::vec3(0, 1, 0)),
			glm::vec3(size));

		entities.AddTag(entity, ddc::Tag::Dynamic);

		dd::BoundSphereComponent& bound_sphere = entities.Add<dd::BoundSphereComponent>(entity);
		bound_sphere.Sphere.Radius = 1.0f;

		dd::PhysicsSphereComponent& physics_sphere = entities.Add<dd::PhysicsSphereComponent>(entity);
		physics_sphere.Sphere.Radius = 1.0f;
		physics_sphere.Elasticity = 0.95f;
		physics_sphere.Mass = 1.0f;

		return entity;
	}

	void TestEntities::CreatePhysicsPlaneTestScene(ddc::EntityLayer& entities)
	{
		float sphere_size = 3;

		dd::Array<glm::vec3, 4> ball_positions;
		ball_positions.Add(glm::vec3(0, 60, -30));
		ball_positions.Add(glm::vec3(0, 60, 30));
		ball_positions.Add(glm::vec3(30, 60, 0));
		ball_positions.Add(glm::vec3(-30, 60, 0));

		dd::Array<ddc::Entity, 4> balls;
		balls.Add(CreateBall(entities, ball_positions[0], glm::vec4(0.2, 0.2, 0.8, 1), sphere_size));
		balls.Add(CreateBall(entities, ball_positions[1], glm::vec4(0.8, 0.2, 0.8, 1), sphere_size));
		balls.Add(CreateBall(entities, ball_positions[2], glm::vec4(0.2, 0.8, 0.2, 1), sphere_size));
		balls.Add(CreateBall(entities, ball_positions[3], glm::vec4(0.2, 0.8, 0.8, 1), sphere_size));

		float static_sphere_size = 100;

		ddr::MeshHandle sphere_mesh("sphere");

		/* STATIC SPHERES
		{
			ddc::Entity sphere = CreateMeshEntity( entities, sphere_mesh, glm::vec4( 0.9, 0.9, 0.9, 1 ), glm::vec3( 0, static_sphere_size * -0.7f, 0 ), glm::quat(), glm::vec3( static_sphere_size ) );
			entities.AddTag( sphere, ddc::Tag::Static );

			dd::PhysicsSphereComponent& physics_sphere = entities.Add<dd::PhysicsSphereComponent>( sphere );
			physics_sphere.Sphere = ddm::Sphere( glm::vec3( 0, 0, 0 ), 1 );
			physics_sphere.Elasticity = 0.95f;
		}

		{
			ddc::Entity sphere = CreateMeshEntity( entities, sphere_mesh, glm::vec4( 0.9, 0.9, 0.9, 1 ), glm::vec3( 0, static_sphere_size * -0.7f, -static_sphere_size * 1.3f ), glm::quat(), glm::vec3( static_sphere_size ) );
			entities.AddTag( sphere, ddc::Tag::Static );

			dd::PhysicsSphereComponent& physics_sphere = entities.Add<dd::PhysicsSphereComponent>( sphere );
			physics_sphere.Sphere = ddm::Sphere( glm::vec3( 0, 0, 0 ), 1 );
			physics_sphere.Elasticity = 0.95f;
		}

		{
			ddc::Entity sphere = CreateMeshEntity( entities, sphere_mesh, glm::vec4( 0.9, 0.9, 0.9, 1 ), glm::vec3( 0, static_sphere_size * -0.7f, static_sphere_size * 1.3f ), glm::quat(), glm::vec3( static_sphere_size ) );
			entities.AddTag( sphere, ddc::Tag::Static );

			dd::PhysicsSphereComponent& physics_sphere = entities.Add<dd::PhysicsSphereComponent>( sphere );
			physics_sphere.Sphere = ddm::Sphere( glm::vec3( 0, 0, 0 ), 1 );
			physics_sphere.Elasticity = 0.95f;
		}*/

		float plane_size = 100;
		auto proto_h = s_entityProtoManager->Create("physics_plane");
		ddc::EntityPrototype* phys_plane_proto = proto_h.Access();

		{
			ddc::Entity plane = CreateMeshEntity(entities, ddr::MeshHandle("quad"), glm::vec4(0.2, 0.8, 0.2, 1), glm::vec3(0), glm::angleAxis(glm::radians(45.0f), glm::vec3(1, 0, 0)), glm::vec3(plane_size));
			entities.AddTag(plane, ddc::Tag::Static);

			dd::PhysicsPlaneComponent& physics_plane = entities.Add<dd::PhysicsPlaneComponent>(plane);
			physics_plane.Plane = ddm::Plane(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			physics_plane.Elasticity = 0.95f;

			phys_plane_proto->PopulateFromEntity(plane);
		}

		{
			ddc::Entity plane = phys_plane_proto->Instantiate(entities);
			dd::ColourComponent* clr = entities.Access<dd::ColourComponent>(plane);
			clr->Colour = glm::vec4(0.8, 0.2, 0.2, 1);

			dd::TransformComponent* transform_cmp = entities.Access<dd::TransformComponent>(plane);
			transform_cmp->Rotation = glm::angleAxis(glm::radians(-45.0f), glm::vec3(1, 0, 0));
			transform_cmp->Update();
		}

		{
			ddc::Entity plane = phys_plane_proto->Instantiate(entities);
			dd::ColourComponent* clr = entities.Access<dd::ColourComponent>(plane);
			clr->Colour = glm::vec4(0.8, 0.8, 0.2, 1);

			dd::TransformComponent* transform_cmp = entities.Access<dd::TransformComponent>(plane);
			transform_cmp->Rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0)) *
				glm::angleAxis(glm::radians(45.0f), glm::vec3(1, 0, 0));

			transform_cmp->Update();
		}

		{
			ddc::Entity plane = phys_plane_proto->Instantiate(entities);
			dd::ColourComponent* clr = entities.Access<dd::ColourComponent>(plane);
			clr->Colour = glm::vec4(0.2, 0.2, 0.8, 1);

			dd::TransformComponent* transform_cmp = entities.Access<dd::TransformComponent>(plane);
			transform_cmp->Rotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 1, 0)) *
				glm::angleAxis(glm::radians(45.0f), glm::vec3(1, 0, 0));

			transform_cmp->Update();
		}

		dd::Service<dd::Input>()->AddHandler(dd::InputAction::RESET_PHYSICS, 
			[balls, ball_positions, &entities]()
			{
				for (size_t i = 0; i < balls.Size(); ++i)
				{
					dd::TransformComponent* transform = entities.Access<dd::TransformComponent>(balls[i]);
					transform->Position = ball_positions[i];
					transform->Update();

					dd::PhysicsSphereComponent* sphere = entities.Access<dd::PhysicsSphereComponent>(balls[i]);
					sphere->Momentum = glm::vec3(0, 0, 0);
					sphere->Resting = false;
				}
			});
	}

	void TestEntities::CreateAxes(ddc::EntityLayer& entities)
	{
		{
			ddc::Entity x_entity = entities.CreateEntity<dd::RayComponent, dd::ColourComponent>();
			entities.AddTag(x_entity, ddc::Tag::Visible);

			dd::RayComponent* x_ray = entities.Access<dd::RayComponent>(x_entity);
			x_ray->Ray = ddm::Ray(glm::vec3(-50, 0, 0), glm::vec3(1, 0, 0), 100);

			dd::ColourComponent* x_colour = entities.Access<dd::ColourComponent>(x_entity);
			x_colour->Colour = glm::vec4(1, 0, 0, 1);
		}

		{
			ddc::Entity y_entity = entities.CreateEntity<dd::RayComponent, dd::ColourComponent>();
			entities.AddTag(y_entity, ddc::Tag::Visible);

			dd::RayComponent* y_ray = entities.Access<dd::RayComponent>(y_entity);
			y_ray->Ray = ddm::Ray(glm::vec3(0, -50, 0), glm::vec3(0, 1, 0), 100);

			dd::ColourComponent* y_colour = entities.Access<dd::ColourComponent>(y_entity);
			y_colour->Colour = glm::vec4(0, 1, 0, 1);
		}

		{
			ddc::Entity z_entity = entities.CreateEntity<dd::RayComponent, dd::ColourComponent>();
			entities.AddTag(z_entity, ddc::Tag::Visible);

			dd::RayComponent* z_ray = entities.Access<dd::RayComponent>(z_entity);
			z_ray->Ray = ddm::Ray(glm::vec3(0, 0, -50), glm::vec3(0, 0, 1), 100);

			dd::ColourComponent* z_colour = entities.Access<dd::ColourComponent>(z_entity);
			z_colour->Colour = glm::vec4(0, 0, 1, 1);
		}
	}
}