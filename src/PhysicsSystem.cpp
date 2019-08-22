//
// PhysicsSystem.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#include "PCH.h"
#include "PhysicsSystem.h"

#include "BVHTree.h"
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
		RequireWrite<dd::TransformComponent>( "dynamic_spheres" );
		RequireWrite<dd::PhysicsSphereComponent>( "dynamic_spheres" );
		RequireTag( ddc::Tag::Dynamic, "dynamic_spheres" );

		RequireRead<dd::TransformComponent>( "static_spheres" );
		RequireRead<dd::PhysicsSphereComponent>( "static_spheres" );
		RequireTag( ddc::Tag::Static, "static_spheres" );

		RequireRead<dd::TransformComponent>( "static_planes" );
		RequireRead<dd::PhysicsPlaneComponent>( "static_planes" );
		RequireTag( ddc::Tag::Static, "static_planes" );
	}

	void PhysicsSystem::Initialize( ddc::EntityLayer& entities )
	{

	}

	static void AddSphereToBVH( BVHTree& bvh, const ddm::Sphere& sphere, const glm::vec3& velocity, const glm::mat4& transform )
	{
		ddm::Sphere transformed = sphere.GetTransformed( transform );
		ddm::AABB bounds( transformed );
		bounds.Expand( bounds.Min + velocity );
		bounds.Expand( bounds.Max + velocity );
		bvh.Add( bounds );
	}

	static bool IntersectMovingSpherePlane( const ddm::Sphere& sphere, const ddm::Plane& plane, const glm::vec3& displacement, float& out_time )
	{
		float distance = plane.DistanceTo( sphere.Centre );
		if( glm::abs( distance ) < sphere.Radius )
		{
			out_time = 0;
			return true;
		}

		float dot = glm::dot( plane.Normal(), displacement );
		if( dot * distance >= 0 )
		{
			return false;
		}

		float r = distance > 0.0f ? sphere.Radius : -sphere.Radius;
		out_time = (r - distance) / dot;

		return true;
	}

	// Real-Time Collision Detection, pg. 224
	static bool IntersectMovingSphereSphere( const ddm::Sphere& s0, const ddm::Sphere& s1, const glm::vec3& v0, const glm::vec3& v1, float& out_time )
	{
		glm::vec3 s = s1.Centre - s0.Centre;
		glm::vec3 v = v1 - v0;

		float r = s0.Radius + s1.Radius;
		float c = glm::dot( s, s ) - r * r;

		if( c < 0.001f )
		{
			// spheres overlapping initially
			out_time = 0;
			return true;
		}

		float a = glm::dot( v, v );
		if( a < 0.001f )
		{
			// spheres not moving relative to each other
			return false;
		}

		float b_index = glm::dot( v, s );
		if( b_index > 0 )
		{
			// spheres not moving towards each other
			return false;
		}

		float d = b_index * b_index - a * c;
		if( d < 0 )
		{
			// no intersection
			return false;
		}

		out_time = (-b_index - std::sqrt( d )) / a;
		return true;
	}

	static void ReflectVelocity( dd::PhysicsSphereComponent& physics, dd::TransformComponent& transform, const ddm::Sphere& sphere, 
		const glm::vec3& normal, float delta_t, float hit_time, float elasticity )
	{
		glm::vec3 velocity = physics.Momentum / physics.Mass;

		ddm::Sphere moving_interp = sphere;
		moving_interp.Centre += velocity * (delta_t * hit_time);

		float speed = glm::length( velocity );
		velocity = glm::reflect( velocity / speed, normal );
		velocity *= speed * elasticity;

		physics.Momentum = velocity * physics.Mass;

		float remainder_t = delta_t * (1 - hit_time);

		transform.Position = moving_interp.Centre + velocity * remainder_t;
		transform.Update();
	}

	static bool IntersectStaticPlanes( const ddc::UpdateDataBuffer& static_planes, dd::PhysicsSphereComponent& moving_physics, dd::TransformComponent& moving_transform, float delta_t )
	{
		ddm::Sphere moving_sphere = moving_physics.Sphere.GetTransformed( moving_transform.Transform() );

		auto transforms = static_planes.Read<dd::TransformComponent>();
		auto physics = static_planes.Read<dd::PhysicsPlaneComponent>();

		for( size_t p = 0; p < static_planes.Size(); ++p )
		{
			const dd::PhysicsPlaneComponent& phys_plane = physics[p];
			ddm::Plane static_plane = phys_plane.Plane.GetTransformed( transforms[p].Transform() );
			glm::vec3 velocity = moving_physics.Momentum / moving_physics.Mass;

			float hit_time = 0;
			if( IntersectMovingSpherePlane( moving_sphere, static_plane, velocity * delta_t, hit_time ) )
			{
				if( hit_time < 0 || hit_time > 1 )
					continue;

				ReflectVelocity( moving_physics, moving_transform, moving_sphere, static_plane.Normal(), delta_t, hit_time, phys_plane.Elasticity * moving_physics.Elasticity );

				return true;
			}
		}

		return false;
	}

	static bool IntersectStaticSpheres( const ddc::UpdateDataBuffer& static_spheres, dd::PhysicsSphereComponent& moving_physics, dd::TransformComponent& moving_transform, float delta_t )
	{
		ddm::Sphere moving_sphere = moving_physics.Sphere.GetTransformed( moving_transform.Transform() );

		auto transforms = static_spheres.Read<dd::TransformComponent>();
		auto physics = static_spheres.Read<dd::PhysicsSphereComponent>();

		for( size_t s = 0; s < static_spheres.Size(); ++s )
		{
			const dd::PhysicsSphereComponent& static_physics = physics[s];

			ddm::Sphere static_sphere = static_physics.Sphere.GetTransformed( transforms[s].Transform() );

			glm::vec3 velocity = moving_physics.Momentum / moving_physics.Mass;

			float hit_time = 0;
			if( IntersectMovingSphereSphere( moving_sphere, static_sphere, velocity * delta_t, glm::vec3( 0 ), hit_time ) )
			{
				if( hit_time < 0 || hit_time > 1 )
					continue;

				glm::vec3 hit_normal = glm::normalize( moving_sphere.Centre - static_sphere.Centre );

				ReflectVelocity( moving_physics, moving_transform, moving_sphere, hit_normal, delta_t, hit_time, static_physics.Elasticity * moving_physics.Elasticity );

				return true;
			}
		}

		return false;
	}

	static std::vector<BVHHandle> s_hits;

	static bool DynamicSpheresCollisions( const ddc::WriteView<dd::TransformComponent>& dynamic_sphere_transforms, 
		const ddc::WriteView<dd::PhysicsSphereComponent>& dynamic_sphere_physics, const dd::BVHTree& bvh, size_t a_index, float delta_t )
	{
		s_hits.reserve( dynamic_sphere_transforms.Size() );
		
		dd::PhysicsSphereComponent& a_physics = dynamic_sphere_physics[a_index];
		dd::TransformComponent& a_transform = dynamic_sphere_transforms[a_index];

		ddm::Sphere a_sphere = a_physics.Sphere.GetTransformed( a_transform.Transform() );

		glm::vec3 a_velocity = a_physics.Momentum / a_physics.Mass;
		glm::vec3 a_displacement = a_velocity * delta_t;

		ddm::Sphere a_expanded = a_sphere;
		a_expanded.Radius += glm::length( a_displacement );

		s_hits.clear();
		bvh.WithinBoundSphere( a_expanded, s_hits );

		for(BVHHandle hit : s_hits )
		{
			size_t b_index = (size_t) hit;
			if( a_index == b_index )
				continue;

			dd::PhysicsSphereComponent& b_physics = dynamic_sphere_physics[b_index];
			dd::TransformComponent& b_transform = dynamic_sphere_transforms[b_index];

			ddm::Sphere b_sphere = b_physics.Sphere.GetTransformed( b_transform.Transform() );

			glm::vec3 b_velocity = b_physics.Momentum / b_physics.Mass;
			glm::vec3 b_displacement = b_velocity * delta_t;

			float hit_time = 0;
			if( IntersectMovingSphereSphere( a_sphere, b_sphere, a_displacement, b_displacement, hit_time ) )
			{
				if( hit_time < 0 || hit_time > 1 )
					continue;

				ddm::Sphere a_interp = a_sphere;
				a_interp.Centre += a_displacement * hit_time;

				ddm::Sphere b_interp = b_sphere;
				b_interp.Centre += b_displacement * hit_time;

				float a_mass = a_physics.Mass;
				float b_mass = b_physics.Mass;
				DD_ASSERT( a_mass > 0 && b_mass > 0 );

				glm::vec3 a_initial = a_velocity;
				glm::vec3 b_initial = b_velocity;

				// https://en.wikipedia.org/wiki/Elastic_collision
				a_velocity = (a_initial * (a_mass - b_mass) + (2.0f * b_mass * b_initial)) / (a_mass + b_mass);
				b_velocity = (b_initial * (b_mass - a_mass) + (2.0f * a_mass * a_initial)) / (a_mass + b_mass);

				float elasticity = a_physics.Elasticity * b_physics.Elasticity;

				a_physics.Momentum = a_velocity * elasticity * a_mass;
				b_physics.Momentum = b_velocity * elasticity * b_mass;

				// offset positions by remainder of delta
				float remainder_t = delta_t * (1 - hit_time);

				a_transform.Position = a_interp.Centre + a_velocity * remainder_t;
				a_transform.Update();

				b_transform.Position = b_interp.Centre + b_velocity * remainder_t;
				b_transform.Update();

				return true;
			}
		}

		return false;
	}

	void PhysicsSystem::Update( const ddc::UpdateData& update )
	{
		DD_PROFILE_SCOPED( PhysicsSystem_Update );

		m_collisions = 0;

		float delta_t = update.Delta();
		
		const auto& static_planes = update.Data( "static_planes" );
		const auto& static_spheres = update.Data( "static_spheres" );
		const auto& dynamic_spheres = update.Data( "dynamic_spheres" );

		auto dynamic_sphere_transforms = dynamic_spheres.Write<dd::TransformComponent>();
		auto dynamic_sphere_physics = dynamic_spheres.Write<dd::PhysicsSphereComponent>();
		
		// build BVH for broadphase
		m_broadphase.Clear();
		m_broadphase.Reserve(dynamic_spheres.Size());

		for( size_t i = 0; i < dynamic_spheres.Size(); ++i )
		{
			glm::vec3 velocity = dynamic_sphere_physics[i].Momentum / dynamic_sphere_physics[i].Mass;

			AddSphereToBVH(m_broadphase, dynamic_sphere_physics[i].Sphere, velocity, dynamic_sphere_transforms[i].Transform());
		}

		m_broadphase.Build();

		for( size_t sphere_idx = 0; sphere_idx < dynamic_spheres.Size(); ++sphere_idx )
		{
			dd::PhysicsSphereComponent& ds_physics = dynamic_sphere_physics[sphere_idx];
			if( ds_physics.Resting )
				continue;

			// calculate universal parameters
			dd::TransformComponent& ds_transform = dynamic_sphere_transforms[sphere_idx];
			ds_physics.Momentum += m_gravity * ds_physics.Mass * delta_t;

			// hit tests
			bool collision = false;
			collision |= IntersectStaticPlanes( static_planes, ds_physics, ds_transform, delta_t );
			collision |= IntersectStaticSpheres( static_spheres, ds_physics, ds_transform, delta_t );
			collision |= DynamicSpheresCollisions( dynamic_sphere_transforms, dynamic_sphere_physics, m_broadphase, sphere_idx, delta_t );

			glm::vec3 velocity = ds_physics.Momentum / ds_physics.Mass;

			if (collision)
			{
				++m_collisions;
			}

			if( !collision )
			{
				ds_transform.Position = ds_transform.Position + velocity * delta_t;
				ds_transform.Update();
			}
			else if( glm::length2( velocity ) < 0.05f )
			{
				ds_physics.Momentum = glm::vec3( 0 );
				ds_physics.Resting = true;
			}
		}
	}

	void PhysicsSystem::DrawDebugInternal()
	{
		ImGui::DragFloat3( "Gravity", glm::value_ptr( m_gravity ) );

		ImGui::Value("Collisions", m_collisions);
	}
}