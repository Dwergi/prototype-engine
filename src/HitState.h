//
// HitState.h - State information for a hit test.
// Copyright (C) Sebastian Nordgren 
// September 17th 2017
//

#pragma once

#include "Entity.h"
#include "Ray.h"

namespace dd
{
	struct HitState
	{
		HitState();
		HitState( dd::Ray ray );
		HitState( dd::Ray ray, float length );
		HitState( const HitState& other );

		void operator=( const HitState& other );

		bool IsHit() const { return m_status == Status::Completed && m_distance < FLT_MAX; }
		bool IsValid() const { return m_status != Status::Invalid; }
		bool IsPending() const { return m_status == Status::Pending; }
		bool IsCompleted() const { return m_status == Status::Completed; }

		void SetCompleted();

		glm::vec3 Position() const
		{
			DD_ASSERT( IsCompleted() );
			return m_ray.Origin() + m_ray.Direction() * m_distance;
		}

		dd::Ray Ray() const { return m_ray; }
		float Length() const { return m_length; }
		float Distance() const { return m_distance; }
		ddc::Entity Entity() const { return m_entity; }

		void RegisterHit( float distance, ddc::Entity entity );

	private:

		enum class Status
		{
			Invalid,
			Pending,
			Completed
		};

		Status m_status { Status::Invalid };
		float m_distance { FLT_MAX };
		dd::Ray m_ray;
		float m_length { FLT_MAX };
		ddc::Entity m_entity;
	};

}