//
// HitTestSystem.cpp - A system to do asynchronous hit tests.
// Copyright (C) Sebastian Nordgren 
// September 17th 2018
//

#include "PCH.h"
#include "HitTestSystem.h"

#include "BoundBoxComponent.h"
#include "BoundSphereComponent.h"
#include "BoundsHelpers.h"
#include "HitTest.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Random.h"
#include "TransformComponent.h"
#include "World.h"

namespace dd
{
	static_assert(sizeof( HitHandle ) == sizeof( uint ));

	HitTestSystem::HitTestSystem() : 
		ddc::System( "Hit Testing" )
	{
		RequireRead<dd::TransformComponent>();
		RequireRead<dd::MeshComponent>();

		OptionalRead<dd::BoundBoxComponent>();
		OptionalRead<dd::BoundSphereComponent>();

		RequireTag( ddc::Tag::Visible );
		RequireTag( ddc::Tag::Static );
	}

	void HitTestSystem::Initialize( ddc::World& world )
	{

	}

	HitHandle HitTestSystem::ScheduleHitTest( const Ray& ray, float length )
	{
		uint index = ~0u;
		if( !m_free.empty() )
		{
			index = m_free.back();
			m_free.pop_back();
		}
		else
		{
			index = m_last;
			++m_last;
		}

		DD_ASSERT( index < MAX_HITS );

		++m_scheduled;

		HitEntry& entry = m_hits[index];
		entry.Handle.ID = index;
		entry.Handle.Version++;
		entry.Handle.Valid = true;
		entry.Handle.Completed = false;
		entry.State = HitResult( ray, length );

		return entry.Handle;
	}

	bool HitTestSystem::FetchResult( HitHandle handle, HitResult& state )
	{
		DD_ASSERT( handle.ID < MAX_HITS );

		HitEntry& entry = m_hits[handle.ID];

		if( entry.Handle.Version == handle.Version &&
			entry.Handle.Valid )
		{
			state = entry.State;
			return true;
		}

		return false;
	}

	void HitTestSystem::ReleaseResult( HitHandle handle )
	{
		DD_ASSERT( handle.ID < MAX_HITS );

		HitEntry& entry = m_hits[handle.ID];
		if( entry.Handle.Valid )
		{
			entry.State = HitResult();
			entry.Handle.Valid = false;

			m_free.push_back( entry.Handle.ID );

			++m_released;
		}
	}

	void HitTestSystem::Update( const ddc::UpdateData& update )
	{
		const ddc::DataBuffer& data = update.Data();

		auto transforms = data.Read<dd::TransformComponent>();
		auto bound_boxes = data.Read<dd::BoundBoxComponent>();
		auto bound_spheres = data.Read<dd::BoundSphereComponent>();

		auto meshes = data.Read<dd::MeshComponent>();
		auto entities = data.Entities();

		for( uint i = 0; i < m_last; ++i )
		{
			HitEntry& entry = m_hits[i];
			if( entry.IsPending() )
			{
				for( uint e = 0; e < data.Size(); ++e )
				{
					dd::AABB aabb;
					dd::Sphere sphere;
					if( !dd::GetWorldBoundBoxAndSphere( bound_boxes.Get( e ), bound_spheres.Get( e ), transforms[ i ].Transform, aabb, sphere ) )
					{
						continue;
					}

					float out_distance = FLT_MAX;
					glm::vec3 out_normal;

					ddr::Mesh* mesh = ddr::Mesh::Get( meshes[e].Mesh );
					if( dd::HitTestMesh( entry.State.Ray(), transforms[e].Transform, sphere, aabb, *mesh, out_distance, out_normal ) )
					{
						entry.State.RegisterHit( out_distance, out_normal, entities[e] );
					}
				}

				entry.Handle.Completed = true;

				++m_executed;
			}
		}
	}

	std::vector<HitHandle> s_spamTests;

	void SpamHitTests( IAsyncHitTest* async_hit_test, int count )
	{
		for( HitHandle handle : s_spamTests )
		{
			HitResult result;
			bool found = async_hit_test->FetchResult( handle, result );
			DD_ASSERT( found );

			async_hit_test->ReleaseResult( handle );
		}

		s_spamTests.clear();

		dd::RandomFloat rng_flt( 0, 1 );

		for( int i = 0; i < count; ++i )
		{
			glm::vec3 origin( rng_flt.Next() * 100, rng_flt.Next() * 100, rng_flt.Next() * 100 );
			glm::vec3 dir( rng_flt.Next(), rng_flt.Next(), rng_flt.Next() );
			dd::Ray ray( origin, dir );

			HitHandle handle = async_hit_test->ScheduleHitTest( ray, rng_flt.Next() * 1000 );

			s_spamTests.push_back( handle );
		}
	}

	void HitTestSystem::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::SliderInt( "Hit Tests", &m_hitTests, 0, MAX_HITS / 2 );

		ImGui::Value( "Scheduled", m_scheduled );
		ImGui::Value( "Executed", m_executed );
		ImGui::Value( "Cleared", m_released );
		ImGui::Value( "Free", (uint) m_free.size() );
		ImGui::Value( "Last", m_last );

		SpamHitTests( this, m_hitTests );
	}
}