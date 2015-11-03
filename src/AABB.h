//
// AABB.h - An axis-aligned bounding box.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

namespace dd
{
	struct AABB
	{
		AABB();
		AABB( const AABB& other );
		~AABB();

		void Expand( const glm::vec3& pt );
		bool Contains( const AABB& other ) const;
		bool Contains( const glm::vec3& pt ) const;
		bool Intersects( const AABB& other ) const;

		glm::vec3 Center() const;
		void GetCorners( glm::vec3 (&corners)[8] ) const;

		glm::vec3 Min;
		glm::vec3 Max;
	};
}