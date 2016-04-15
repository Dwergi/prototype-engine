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

		Plane();
		Plane( const glm::vec3& point, const glm::vec3& normal );
		Plane( float a, float b, float c, float d );
		Plane( const glm::vec3& pt1, const glm::vec3& pt2, const glm::vec3& pt3 );
		Plane( const Plane& other );
		~Plane();
		
		glm::vec3 Normal() const;
		glm::vec3 Origin() const;

		float DistanceTo( const glm::vec3& point ) const;

	private:

		glm::vec4 m_plane;
	};
}
