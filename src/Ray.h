//
// Ray.h - A ray.
// Copyright (C) Sebastian Nordgren 
// April 24th 2017
//

#pragma once

namespace ddm
{
	struct Ray
	{
		Ray();
		Ray( const ddm::Ray& other );
		Ray( const glm::vec3& origin, const glm::vec3& direction, float length = FLT_MAX );

		void operator=( const ddm::Ray& other );

		//
		// The origin of the ray.
		//
		glm::vec3 Origin() const { return m_origin; }

		//
		// The normalized direction of the ray.
		//
		glm::vec3 Direction() const { return m_direction; }

		//
		// Does this ray have a length set?
		//
		bool HasLength() const { return Length != FLT_MAX; }

		//
		// The component-wise inverse of the direction. 
		// Note: Not actually normalized, nor necessarily valid floats - ie. may contain INFs or NaNs.
		//
		glm::vec3 InverseDir() const { return m_invDir; }

		float Length { FLT_MAX };

		DD_CLASS( ddm::Ray )
		{
			DD_METHOD( ddm::Ray, Origin );
			DD_METHOD( ddm::Ray, Direction );
			DD_METHOD( ddm::Ray, InverseDir );
		}

	private:

		glm::vec3 m_origin;
		glm::vec3 m_direction;

		glm::vec3 m_invDir;

		void SetDirection( const glm::vec3& dir );
	};
}
