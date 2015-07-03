//
// Bounding Box.h - A naive implementation of an octree.
// Copyright (C) Sebastian Nordgren 
// February 12th 2015
//

#pragma once

#include "Vector.h"

namespace dd
{
	class BoundingBox
	{
	public:

		BoundingBox();
		BoundingBox( const dd::Vector4& min, const dd::Vector4& max );
		BoundingBox( const BoundingBox& other );

		BoundingBox& operator=( const BoundingBox& other );

		//
		// Expand the bounds to include the given point.
		//
		void Include( const dd::Vector4& pos );

		//
		// Expand the bounds to include the given bounding box.
		//
		void Include( const BoundingBox& box );

		//
		// Check if the given point is inside the bounds.
		//
		bool Contains( const dd::Vector4& pos ) const;

		//
		// Check if this bounding box with another.
		//
		bool Intersects( const BoundingBox& other ) const;

		//
		// Get the center point of this bounding box.
		//
		dd::Vector4 GetCenter() const;

	private:

		dd::Vector4 m_minimum;
		dd::Vector4 m_maximum;

		bool m_initialized;
	};
}