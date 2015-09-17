//
// Vector.h - A vector.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Vector.h"

namespace dd
{

	const float VectorBase::GrowthFactor = 2.0f;
	const uint VectorBase::GrowthFudge = 8;

	VectorBase::VectorBase()
		: m_size( 0 ),
		m_capacity( 0 )
	{

	}

	uint VectorBase::Size() const
	{
		return m_size;
	}
}