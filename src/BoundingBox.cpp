#include "PrecompiledHeader.h"
#include "BoundingBox.h"

BoundingBox::BoundingBox()
	: m_initialized( false )
{

}

BoundingBox::BoundingBox( const Vector4& min, const Vector4& max )
	: m_initialized( true )
{
	m_minimum = min;
	m_maximum = max;
}

BoundingBox::BoundingBox( const BoundingBox& other )
	: m_minimum( other.m_minimum ),
	m_maximum( other.m_maximum ),
	m_initialized( other.m_initialized )
{
	
}

BoundingBox& BoundingBox::operator=( const BoundingBox& other )
{
	m_minimum = other.m_minimum;
	m_maximum = other.m_maximum;
	m_initialized = other.m_initialized;

	return *this;
}

void BoundingBox::Include( const Vector4& pos )
{
	if( !m_initialized )
	{
		m_minimum = pos;
		m_maximum = pos;

		return;
	}

	m_minimum.X = std::min( m_minimum.X, pos.X );
	m_minimum.Y = std::min( m_minimum.Y, pos.Y );
	m_minimum.Z = std::min( m_minimum.Z, pos.Z );

	m_maximum.X = std::max( m_maximum.X, pos.X );
	m_maximum.Y = std::max( m_maximum.Y, pos.Y );
	m_maximum.Z = std::max( m_maximum.Z, pos.Z );

	m_initialized = true;
}

void BoundingBox::Include( const BoundingBox& box )
{
	Include( box.m_minimum );
	Include( box.m_maximum );
}

bool BoundingBox::Contains( const Vector4& pos ) const
{
	return pos >= m_minimum && pos <= m_maximum;
}

bool BoundingBox::Intersects( const BoundingBox& other ) const
{
	return m_maximum > other.m_minimum && m_minimum < other.m_maximum;
}

Vector4 BoundingBox::GetCenter() const
{
	return (m_minimum + m_maximum) * 0.5f;
}