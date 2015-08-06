//
// Vector4.h - Vector implementations.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#include "PrecompiledHeader.h"
#include "Vector4.h"

//
// Constructors
//
dd::Vector4::Vector4()
{

}

dd::Vector4::Vector4( float x, float y, float z, float w )
	: X( x ), Y( y ), Z( z ), W( w )
{

}

dd::Vector4::~Vector4()
{

}

dd::Vector4::Vector4( const dd::Vector4& other )
{
	for( int i = 0; i < DEGREE; ++i )
	{
		m_data[ i ] = other.m_data[ i ];
	}
}

dd::Vector4& dd::Vector4::operator=( const dd::Vector4& other )
{
	for( int i = 0; i < DEGREE; ++i )
	{
		m_data[ i ] = other.m_data[ i ];
	}

	return *this;
}

//
// Scalar operators
//
dd::Vector4 dd::Vector4::operator+( float scalar ) const
{
	dd::Vector4 tmp( *this );
	tmp += scalar;

	return tmp;
}

dd::Vector4 dd::Vector4::operator-( float scalar ) const
{
	dd::Vector4 tmp( *this );
	tmp -= scalar;

	return tmp;
}

dd::Vector4 dd::Vector4::operator*( float scalar ) const
{
	dd::Vector4 tmp( *this );
	tmp *= scalar;

	return tmp;
}

dd::Vector4 dd::Vector4::operator/( float scalar ) const
{
	dd::Vector4 tmp( *this );
	tmp /= scalar;

	return tmp;
}

dd::Vector4& dd::Vector4::operator+=( float scalar )
{
	for( float& comp : m_data )
		comp += scalar;

	return *this;
}

dd::Vector4& dd::Vector4::operator-=( float scalar )
{
	for( float& comp : m_data )
		comp -= scalar;

	return *this;
}

dd::Vector4& dd::Vector4::operator*=( float scalar )
{
	for( float& comp : m_data )
		comp *= scalar;

	return *this;
}

dd::Vector4& dd::Vector4::operator/=( float scalar )
{
	for( float& comp : m_data )
		comp /= scalar;

	return *this;
}

//
// Component-wise vector operators
//
dd::Vector4 dd::Vector4::operator+( const dd::Vector4& other ) const
{
	dd::Vector4 tmp( *this );
	tmp += other;

	return tmp;
}

dd::Vector4 dd::Vector4::operator-( const dd::Vector4& other ) const
{
	dd::Vector4 tmp( *this );
	tmp -= other;

	return tmp;
}

dd::Vector4 dd::Vector4::operator*( const dd::Vector4& other ) const
{
	dd::Vector4 tmp( *this );
	tmp *= other;

	return tmp;
}

dd::Vector4 dd::Vector4::operator/( const dd::Vector4& other ) const
{
	dd::Vector4 tmp( *this );
	tmp /= other;

	return tmp;
}

dd::Vector4& dd::Vector4::operator+=( const dd::Vector4& other )
{
	for( int i = 0; i < DEGREE; ++i )
		m_data[ i ] += other.m_data[ i ];

	return *this;
}

dd::Vector4& dd::Vector4::operator-=( const dd::Vector4& other )
{
	for( int i = 0; i < DEGREE; ++i )
		m_data[ i ] -= other.m_data[ i ];

	return *this;
}

dd::Vector4& dd::Vector4::operator*=( const dd::Vector4& other )
{
	for( int i = 0; i < DEGREE; ++i )
		m_data[ i ] += other.m_data[ i ];

	return *this;
}

dd::Vector4& dd::Vector4::operator/=( const dd::Vector4& other )
{
	for( int i = 0; i < DEGREE; ++i )
		m_data[ i ] += other.m_data[ i ];

	return *this;
}

//
// Comparisons
//
bool dd::Vector4::operator==( const dd::Vector4& other ) const
{
	return X == other.X && Y == other.Y && Z == other.Z;
}

bool dd::Vector4::operator>( const dd::Vector4& other ) const
{
	return X > other.X && Y > other.Y && Z > other.Z;
}

bool dd::Vector4::operator<( const dd::Vector4& other ) const
{
	return X < other.X && Y < other.Y && Z < other.Z;
}

bool dd::Vector4::operator<=( const dd::Vector4& other ) const
{
	return X <= other.X && Y <= other.Y && Z <= other.Z;
}

bool dd::Vector4::operator>=( const dd::Vector4& other ) const
{
	return X >= other.X && Y >= other.Y && Z >= other.Z;
}

//
// Vector math
//
float dd::Vector4::length() const
{
	return sqrt( lengthSq() );
}

float dd::Vector4::lengthSq() const
{
	float result = 0;

	for( int i = 0; i < DEGREE; ++i )
	{
		result += m_data[ i ] * m_data[ i ];
	}

	return result;
}

float dd::Vector4::distance( const dd::Vector4& other ) const
{
	return sqrt( distanceSq( other ) );
}

float dd::Vector4::distanceSq( const dd::Vector4& other ) const
{
	dd::Vector4 diff = *this - other;
	
	return diff.lengthSq();
}


float dd::Vector4::dot( const dd::Vector4& other ) const
{
	float result = 0;

	for( int i = 0; i < DEGREE; ++i )
	{
		result += m_data[ i ] * other.m_data[ i ];
	}

	return result;
}

dd::Vector4 dd::Vector4::cross( const dd::Vector4& other ) const
{
	dd::Vector4 result;

	result.X = Y * other.Z - other.Y * Z;
	result.Y = Z * other.X - other.Z * X;
	result.Z = X * other.Y - other.X * Y;

	return result;
}

dd::Vector4 dd::Vector4::getNormalized() const
{
	dd::Vector4 result( *this );
	result.normalize();

	return result;
}

dd::Vector4& dd::Vector4::normalize()
{
	float length = this->length();

	for( int i = 0; i < DEGREE; ++i )
	{
		m_data[ i ] /= length;
	}

	return *this;
}

float& dd::Vector4::operator[]( unsigned int component )
{
	return m_data[ component ];
}

float dd::Vector4::operator[]( unsigned int component ) const
{
	return m_data[ component ];
}