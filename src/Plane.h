//
// Plane.h - A simple plane defined by the plane's normalized form.
// Copyright (C) Sebastian Nordgren 
// November 4th 2015
//

#pragma once

namespace ddm
{
	struct Plane
	{
		Plane() {}
		Plane( const glm::vec4& params );
		Plane( const glm::vec3& point, const glm::vec3& normal );
		Plane( float a, float b, float c, float d );
		Plane( const glm::vec3& pt1, const glm::vec3& pt2, const glm::vec3& pt3 );
		Plane( const Plane& other );
		
		glm::vec3 Normal() const { return Parameters.xyz(); }
		glm::vec3 Origin() const { return Normal() * Parameters.w; }

		float DistanceTo( const glm::vec3& point ) const;
		Plane GetTransformed( const glm::mat4& transform ) const;

		glm::vec4 Parameters;

		DD_CLASS( ddm::Plane )
		{
			DD_COMPONENT();
			DD_MEMBER( ddm::Plane, Parameters );
		}
	};
}
