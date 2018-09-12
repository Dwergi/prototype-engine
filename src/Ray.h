//
// Ray.h - A ray wrapper class.
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

		glm::vec3 Direction() const { return m_direction; }
		void SetDirection( const glm::vec3& direction );

		glm::vec3 Origin() const { return m_origin; }
		void SetOrigin( const glm::vec3& origin ) { m_origin = origin; }

		glm::vec3 InverseDir() const { return m_invDir; }

	private:

		glm::vec3 m_origin;
		glm::vec3 m_direction;
		glm::vec3 m_invDir;
	};
}
