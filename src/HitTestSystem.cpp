//
// HitTestSystem.cpp - A system to do asynchronous hit tests.
// Copyright (C) Sebastian Nordgren 
// September 17th 2018
//

#include "PrecompiledHeader.h"
#include "HitTestSystem.h"

#include "BoundsComponent.h"
#include "HitTest.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Random.h"
#include "TransformComponent.h"
#include "World.h"

#include "imgui/imgui.h"

namespace dd
{
	HitTestSystem::HitTestSystem() : 
		ddc::System( "Hit Testing" )
	{
		RequireRead<dd::TransformComponent>();
		RequireRead<dd::BoundsComponent>();
		RequireRead<dd::MeshComponent>();

		RequireTag( ddc::Tag::Visible );
	}

	void HitTestSystem::Initialize( ddc::World& world )
	{

	}

	const HitState& HitTestSystem::ScheduleHitTest( const Ray& ray, float length )
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

		m_hits[index] = HitState( ray, length );
		return m_hits[index];
	}

	bool HitTestEntity( const Ray& ray, const TransformComponent& transform, const BoundsComponent& bounds,
		const MeshComponent& mesh_cmp, float& out_distance )
	{
		ddr::Mesh* mesh = ddr::Mesh::Get( mesh_cmp.Mesh );
		if( mesh == nullptr )
			return false;

		return dd::HitTestMesh( ray, transform.World, bounds.WorldSphere, bounds.WorldBox, *mesh, out_distance );
	}

	void HitTestSystem::Update( const ddc::UpdateData& data )
	{
		// clear out completed hit tests
		for( uint i = 0; i < m_last; ++i )
		{
			if( m_hits[i].IsCompleted() )
			{
				m_hits[i] = HitState();
				m_free.push_back( i );

				++m_cleared;
			}
		}

		auto transforms = data.Read<dd::TransformComponent>();
		auto bounds = data.Read<dd::BoundsComponent>();
		auto meshes = data.Read<dd::MeshComponent>();
		auto entities = data.Entities();

		for( uint i = 0; i < m_last; ++i )
		{
			if( m_hits[i].IsPending() )
			{
				for( uint entity = 0; entity < entities.Size(); ++entity )
				{
					float out_distance = FLT_MAX;
					if( HitTestEntity( m_hits[i].Ray(), transforms[entity], bounds[entity], meshes[entity], out_distance ) )
					{
						m_hits[i].RegisterHit( out_distance, entities[entity] );
					}
				}

				m_hits[i].SetCompleted();

				++m_executed;
			}
		}
	}

	void SpamHitTests( IAsyncHitTest* async_hit_test, int count )
	{
		dd::RandomFloat rng_flt( 0, 1 );

		for( int i = 0; i < count; ++i )
		{
			glm::vec3 origin( rng_flt.Next() * 100, rng_flt.Next() * 100, rng_flt.Next() * 100 );
			glm::vec3 dir( rng_flt.Next(), rng_flt.Next(), rng_flt.Next() );
			dd::Ray ray( origin, dir );

			async_hit_test->ScheduleHitTest( ray, rng_flt.Next() * 1000 );
		}
	}

	void HitTestSystem::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::SliderInt( "Hit Tests", &m_hitTests, 0, MAX_HITS / 2 );

		ImGui::Value( "Scheduled", m_scheduled );
		ImGui::Value( "Executed", m_executed );
		ImGui::Value( "Cleared", m_cleared );
		ImGui::Value( "Free", (uint) m_free.size() );
		ImGui::Value( "Last", m_last );

		DD_ASSERT( m_scheduled == m_executed );

		m_scheduled = 0;
		m_executed = 0;
		m_cleared = 0;

		SpamHitTests( this, m_hitTests );
	}
}