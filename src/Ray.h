//
// Ray.h - A ray.
// Copyright (C) Sebastian Nordgren 
// April 24th 2017
//

#pragma once

namespace dd
{
	struct Ray
	{
		Ray();
		Ray( const Ray& other );
		Ray( const glm::vec3& origin, const glm::vec3& direction );

		void operator=( const Ray& other );

		//
		// The origin of the ray.
		//
		glm::vec3 Origin() const { return m_origin; }

		//
		// The normalized direction of the ray.
		//
		glm::vec3 Direction() const { return m_direction; }

		//
		// The component-wise inverse of the direction. 
		// Note: Not actually normalized, nor necessarily valid floats - ie. may contain INFs or NaNs.
		//
		glm::vec3 InverseDir() const { return m_invDir; }

	private:

		glm::vec3 m_origin;
		glm::vec3 m_direction;
		glm::vec3 m_invDir;

		void SetDirection( const glm::vec3& dir );
	};
}
