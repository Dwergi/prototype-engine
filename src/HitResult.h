//
// HitResult.h - State information for a hit test.
// Copyright (C) Sebastian Nordgren 
// September 17th 2017
//

#pragma once

#include "Entity.h"
#include "Ray.h"

namespace dd
{
	struct HitHandle
	{
		union
		{
			struct
			{
				uint ID : 12;
				uint Version : 18;
				uint Valid : 1;
				uint Completed : 1;
			};

			uint Handle { 0 };
		};

		DD_CLASS( dd::HitHandle )
		{
			DD_MEMBER( dd::HitHandle, Handle );
		}
	};

	struct HitResult
	{
		HitResult();
		HitResult( dd::Ray ray );
		HitResult( dd::Ray ray, float length );
		HitResult( const HitResult& other );

		void operator=( const HitResult& other );

		glm::vec3 Position() const
		{
			return m_ray.Origin() + m_ray.Direction() * m_distance;
		}

		glm::vec3 Normal() const
		{
			return m_normal;
		}

		dd::Ray Ray() const { return m_ray; }
		float Length() const { return m_length; }
		float Distance() const { return m_distance; }
		ddc::Entity Entity() const { return m_entity; }

		void RegisterHit( float distance, glm::vec3 normal, ddc::Entity entity );

	private:

		float m_distance { FLT_MAX };
		dd::Ray m_ray;
		float m_length { FLT_MAX };
		glm::vec3 m_normal;
		ddc::Entity m_entity;
	};

}