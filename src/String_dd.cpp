//
// String_dd.cpp - String implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "String_dd.h"

const float GrowthFactor = 2.0f;

dd::StringBase::StringBase()
{
	Initialize();
}

dd::StringBase::StringBase( const StringBase& other )
{
	Initialize();

	SetString( other.m_buffer, other.m_length );
}

dd::StringBase::StringBase( const char* data, uint length )
{
	Initialize();

	SetString( data, length );
}

dd::StringBase::~StringBase()
{
	// delete stack buffer
	if( m_buffer != m_stackBuffer )
	{
		delete[] m_buffer;
	}

	Initialize();
}

void dd::StringBase::Initialize()
{
	m_length = 0;

	m_capacity = 0;
	m_buffer = nullptr;

	m_stackCapacity = 0;
	m_stackBuffer = nullptr;
}

const char* dd::StringBase::c_str() const
{
	// check that we're null-terminated
	ASSERT( m_buffer[ m_length ] == 0 );

	return m_buffer;
}

dd::StringBase& dd::StringBase::operator=( const StringBase& other )
{
	SetString( other.m_buffer, other.m_length );

	return *this;
}

void dd::StringBase::SetString( const char* data, uint length )
{
	ASSERT( m_buffer != nullptr );
	ASSERT( data != nullptr );

	Resize( length + 1 );

	memcpy( m_buffer, data, length );

	// null-terminate
	m_buffer[ m_length ] = 0;
}

//
// Resize to contain a string of the given length. May expand to be larger than the given size.
// Never shrinks a string.
//
void dd::StringBase::Resize( uint length )
{
	// keep growing string until we hit a size that fits
	uint new_capacity = m_capacity;
	while( length > new_capacity )
	{
		new_capacity = (uint) (new_capacity * GrowthFactor);
	}

	if( m_capacity == new_capacity )
		return;

	char* old_buffer = m_buffer;

	m_buffer = new char[ new_capacity ];

	// copy old data over if required
	if( m_length > 0 )
		SetString( old_buffer, m_length );

	// delete old buffer if not stack-allocated
	if( old_buffer != m_stackBuffer )
	{
		delete[] old_buffer;
	}
}