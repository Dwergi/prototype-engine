#include "PrecompiledHeader.h"

//
// Constructors
//
Vector4::Vector4()
{

}

Vector4::Vector4( float x, float y, float z, float w )
	: X( x ), Y( y ), Z( z ), W( w )
{

}

Vector4::~Vector4()
{

}

Vector4::Vector4( const Vector4& other )
{
	for( int i = 0; i < DEGREE; ++i )
	{
		m_data[ i ] = other.m_data[ i ];
	}
}

Vector4& Vector4::operator=( const Vector4& other )
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
Vector4 Vector4::operator+( float scalar ) const
{
	Vector4 tmp( *this );
	tmp += scalar;

	return tmp;
}

Vector4 Vector4::operator-( float scalar ) const
{
	Vector4 tmp( *this );
	tmp -= scalar;

	return tmp;
}

Vector4 Vector4::operator*( float scalar ) const
{
	Vector4 tmp( *this );
	tmp *= scalar;

	return tmp;
}

Vector4 Vector4::operator/( float scalar ) const
{
	Vector4 tmp( *this );
	tmp /= scalar;

	return tmp;
}

Vector4& Vector4::operator+=( float scalar )
{
	for( float& comp : m_data )
		comp += scalar;

	return *this;
}

Vector4& Vector4::operator-=( float scalar )
{
	for( float& comp : m_data )
		comp -= scalar;

	return *this;
}

Vector4& Vector4::operator*=( float scalar )
{
	for( float& comp : m_data )
		comp *= scalar;

	return *this;
}

Vector4& Vector4::operator/=( float scalar )
{
	for( float& comp : m_data )
		comp /= scalar;

	return *this;
}

//
// Component-wise vector operators
//
Vector4 Vector4::operator+( const Vector4& other ) const
{
	Vector4 tmp( *this );
	tmp += other;

	return tmp;
}

Vector4 Vector4::operator-( const Vector4& other ) const
{
	Vector4 tmp( *this );
	tmp -= other;

	return tmp;
}

Vector4 Vector4::operator*( const Vector4& other ) const
{
	Vector4 tmp( *this );
	tmp *= other;

	return tmp;
}

Vector4 Vector4::operator/( const Vector4& other ) const
{
	Vector4 tmp( *this );
	tmp /= other;

	return tmp;
}

Vector4& Vector4::operator+=( const Vector4& other )
{
	for( int i = 0; i < DEGREE; ++i )
		m_data[ i ] += other.m_data[ i ];

	return *this;
}

Vector4& Vector4::operator-=( const Vector4& other )
{
	for( int i = 0; i < DEGREE; ++i )
		m_data[ i ] -= other.m_data[ i ];

	return *this;
}

Vector4& Vector4::operator*=( const Vector4& other )
{
	for( int i = 0; i < DEGREE; ++i )
		m_data[ i ] += other.m_data[ i ];

	return *this;
}

Vector4& Vector4::operator/=( const Vector4& other )
{
	for( int i = 0; i < DEGREE; ++i )
		m_data[ i ] += other.m_data[ i ];

	return *this;
}

//
// Comparisons
//
bool Vector4::operator==( const Vector4& other ) const
{
	return X == other.X && Y == other.Y && Z == other.Z;
}

bool Vector4::operator>( const Vector4& other ) const
{
	return X > other.X && Y > other.Y && Z > other.Z;
}

bool Vector4::operator<( const Vector4& other ) const
{
	return X < other.X && Y < other.Y && Z < other.Z;
}

bool Vector4::operator<=( const Vector4& other ) const
{
	return X <= other.X && Y <= other.Y && Z <= other.Z;
}

bool Vector4::operator>=( const Vector4& other ) const
{
	return X >= other.X && Y >= other.Y && Z >= other.Z;
}

//
// Vector math
//
float Vector4::length() const
{
	return sqrt( lengthSq() );
}

float Vector4::lengthSq() const
{
	float result = 0;

	for( int i = 0; i < DEGREE; ++i )
	{
		result += m_data[ i ] * m_data[ i ];
	}

	return result;
}

float Vector4::distance( const Vector4& other ) const
{
	return sqrt( distanceSq( other ) );
}

float Vector4::distanceSq( const Vector4& other ) const
{
	Vector4 diff = *this - other;
	
	return diff.lengthSq();
}


float Vector4::dot( const Vector4& other ) const
{
	float result = 0;

	for( int i = 0; i < DEGREE; ++i )
	{
		result += m_data[ i ] * other.m_data[ i ];
	}

	return result;
}

Vector4 Vector4::cross( const Vector4& other ) const
{
	Vector4 result;

	result.X = Y * other.Z - other.Y * Z;
	result.Y = Z * other.X - other.Z * X;
	result.Z = X * other.Y - other.X * Y;

	return result;
}

Vector4 Vector4::getNormalized() const
{
	Vector4 result( *this );
	result.normalize();

	return result;
}

Vector4& Vector4::normalize()
{
	float length = this->length();

	for( int i = 0; i < DEGREE; ++i )
	{
		m_data[ i ] /= length;
	}

	return *this;
}

float& Vector4::operator[]( unsigned int component )
{
	return m_data[ component ];
}

float Vector4::operator[]( unsigned int component ) const
{
	return m_data[ component ];
}