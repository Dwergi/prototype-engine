//
// Plane.h - A simple plane defined by the plane's normalized form.
// Copyright (C) Sebastian Nordgren 
// November 4th 2015
//

#pragma once

namespace dd
{
	class Plane
	{
	public:

		Plane( const glm::vec3& point, const glm::vec3& normal );
		Plane( const Plane& other );
		~Plane();
		
		glm::vec3 Normal() const;
		glm::vec3 Origin() const;

		float DistanceTo( const glm::vec3& point ) const;

	private:

		// x, y and z are the normal of the plane, w is the negative distance along the normal to the origin
		glm::vec4 m_plane;
	};
}
