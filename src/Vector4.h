//
// Vector4.h - Vector implementations.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#pragma once

namespace dd 
{
	class Vector4
	{
	public:

		static const int DEGREE = 4;

		union
		{
			struct
			{
				float m_data[ 4 ];
			};
			struct
			{
				float X, Y, Z, W;
			};
		};

		//
		// Constructors
		//
		Vector4();
		Vector4( float x, float y, float z, float w = 0.0f );
		Vector4( const Vector4& other );
		~Vector4();

		Vector4& operator=( const Vector4& other );

		//
		// Scalar operators
		//
		Vector4 operator+( float scalar ) const;
		Vector4 operator-( float scalar ) const;
		Vector4 operator*( float scalar ) const;
		Vector4 operator/( float scalar ) const;

		Vector4& operator+=( float scalar );
		Vector4& operator-=( float scalar );
		Vector4& operator*=( float scalar );
		Vector4& operator/=( float scalar );

		//
		// Component-wise vector operators
		//
		Vector4 operator+( const Vector4& other ) const;
		Vector4 operator-( const Vector4& other ) const;
		Vector4 operator*( const Vector4& other ) const;
		Vector4 operator/( const Vector4& other ) const;

		Vector4& operator+=( const Vector4& other );
		Vector4& operator-=( const Vector4& other );
		Vector4& operator*=( const Vector4& other );
		Vector4& operator/=( const Vector4& other );

		//
		// Comparisons
		//
		bool operator==( const Vector4& other ) const;
		bool operator>( const Vector4& other ) const;
		bool operator<( const Vector4& other ) const;
		bool operator<=( const Vector4& other ) const;
		bool operator>=( const Vector4& other ) const;

		//
		// Vector math
		//
		float length() const;
		float lengthSq() const;

		float distance( const Vector4& other ) const;
		float distanceSq( const Vector4& other ) const;

		float dot( const Vector4& other ) const;
		Vector4 cross( const Vector4& other ) const;

		Vector4 getNormalized() const;
		Vector4& normalize();

		//
		// Element accessors
		// 
		float& operator[]( unsigned int component );
		float operator[]( unsigned int component ) const;

		BEGIN_SCRIPT_OBJECT( Vector4 )
			MEMBER( X );
			MEMBER( Y );
			MEMBER( Z );
			MEMBER( W );
		END_MEMBERS
	};
}