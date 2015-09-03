//
// Vector.h - A vector.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "Vector.h"

const float dd::VectorBase::GrowthFactor = 1.75f;
const uint dd::VectorBase::GrowthFudge = 8;

dd::VectorBase::VectorBase()
	: m_size( 0 ),
	m_capacity( 0 )
{

}

uint dd::VectorBase::Size() const
{
	return m_size;
}