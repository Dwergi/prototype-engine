//
// SharedString.cpp - A shared string implementation.
// Copyright (C) Sebastian Nordgren 
// November 5th 2015
//

#include "PrecompiledHeader.h"
#include "SharedString.h"

namespace dd
{
	DenseMap<uint64, std::shared_ptr<const char>> SharedString::s_instances;

	SharedString::SharedString()
		: m_length( 0 ),
		m_hash( 0 )
	{

	}

	SharedString::SharedString( const char* str )
	{
		Assign( str );
	}

	SharedString::SharedString( const String& str )
	{
		Assign( str.c_str() );
	}

	SharedString::SharedString( const SharedString& other )
		: m_ptr( other.m_ptr ),
		m_length( other.m_length ),
		m_hash( other.m_hash )
	{

	}

	SharedString::SharedString( SharedString&& other )
		: m_ptr( other.m_ptr ),
		m_length( other.m_length ),
		m_hash( other.m_hash )
	{
		other.Clear();
	}

	SharedString::~SharedString()
	{
		Clear();
	}

	SharedString& SharedString::operator=( const String& other )
	{
		Clear();
		Assign( other.c_str() );
		return *this;
	}

	SharedString& SharedString::operator=( const SharedString& other )
	{
		m_ptr = other.m_ptr;
		m_length = other.m_length;
		m_hash = other.m_hash;

		return *this;
	}

	SharedString& SharedString::operator=( const char* other )
	{
		Clear();
		Assign( other );
		return *this;
	}

	bool SharedString::operator==( const SharedString& other ) const
	{
		return m_ptr == other.m_ptr;
	}

	bool SharedString::operator==( const char* other ) const
	{
		return strcmp( m_ptr.get(), other ) == 0;
	}
	
	int SharedString::Find( const char* other, uint offset ) const
	{
		DD_ASSERT( m_ptr.get() != nullptr );
		DD_ASSERT( other != nullptr );
		DD_ASSERT( offset >= 0 && offset < m_length );

		const char* res = strstr( m_ptr.get(), other );
		if( res == nullptr )
			return -1;

		uint location = (uint) (res - m_ptr.get());

		return location;
	}

	int SharedString::Find( const SharedString& other, uint offset ) const
	{
		return Find( other.c_str(), offset );
	}

	char SharedString::operator[]( uint index ) const
	{
		DD_ASSERT( index >= 0 && index < m_length );
		return m_ptr.get()[index];
	}

	void SharedString::Clear()
	{
		m_ptr.reset();
		m_length = 0;

		if( m_hash == 0 )
			return;

		std::shared_ptr<const char>& instance = s_instances[m_hash];
		if( instance.unique() )
		{
			// only instance remaining
			s_instances.Remove( m_hash );
		}

		m_hash = 0;
	}

	void SharedString::Assign( const char* str )
	{
		m_length = (uint) strlen( str );
		m_hash = HashString( str, m_length );

		std::shared_ptr<const char>* existing = s_instances.Find( m_hash );
		if( existing != nullptr )
		{
			m_ptr = *existing;
		}
		else
		{
			// create a copy, then store it in the hash map
			char* buffer = new char[m_length + 1];

			memcpy( buffer, str, m_length );
			buffer[m_length] = 0;

			m_ptr.reset( buffer );

			s_instances.Add( m_hash, m_ptr );
		}
	}
}