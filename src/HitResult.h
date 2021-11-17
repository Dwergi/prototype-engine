//
// HitResult.h - State information for a hit test.
// Copyright (C) Sebastian Nordgren 
// September 17th 2017
//

#pragma once

#include "ddm/Ray.h"

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

		DD_BEGIN_CLASS(dd::HitHandle)
			DD_MEMBER(Handle);
		DD_END_CLASS()
	};

	struct HitResult
	{
		HitResult();
		HitResult(ddm::Ray ray);
		HitResult(const HitResult& other);

		void operator=(const HitResult& other);

		bool IsValid() const { return m_distance < FLT_MAX; }

		glm::vec3 Position() const { return m_ray.Origin() + m_ray.Direction() * m_distance; }
		glm::vec3 Normal() const { return m_normal; }
		ddm::Ray Ray() const { return m_ray; }
		float Distance() const { return m_distance; }
		ddc::Entity Entity() const { return m_entity; }

		void RegisterHit(float distance, glm::vec3 normal, ddc::Entity entity);

		DD_BEGIN_CLASS(dd::HitResult)
		DD_END_CLASS()

	private:

		float m_distance { FLT_MAX };
		ddm::Ray m_ray;
		glm::vec3 m_normal;
		ddc::Entity m_entity;
	};

}