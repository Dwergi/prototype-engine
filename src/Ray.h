//
// Ray.h - A ray wrapper class.
// Copyright (C) Sebastian Nordgren 
// April 24th 2017
//

#pragma once

namespace dd
{
	class Ray
	{
	public:

		Ray();
		Ray( const glm::vec3& origin, const glm::vec3& direction );

		glm::vec3 GetDirection() const { return m_direction; }
		void SetDirection( const glm::vec3& direction );

		glm::vec3 GetOrigin() const { return m_origin; }
		void SetOrigin( const glm::vec3& origin ) { m_origin = origin; }

	private:

		glm::vec3 m_direction;
		glm::vec3 m_origin;
	};
}
