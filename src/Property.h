//
// Property.h - A wrapper around a member that can be used to create a property editor.
// Copyright (C) Sebastian Nordgren 
// September 2nd 2015
// Significantly influenced by Randy Gaul (http://RandyGaul.net)
//

#pragma once

namespace dd
{
	class Property
	{
	public:
		Property();
		Property( const Function& get, const Function& set );

		// Get or set the member this property wraps
		template <typename T>
		void Get( T& ret );
		template <typename T>
		void Set( T val );

		// Bind an object to use this property with
		template <typename T>
		void Bind( T& self );

	private:
		Function m_get;
		Function m_set;
	};

	template <typename T>
	void Property::Get( T& ret )
	{
		Variable v( ret );
		return m_get( v );
	}

	template <typename T>
	void Property::Set( T val )
	{
		m_set( val );
	}

	template <typename T>
	void Property::Bind( T& self )
	{
		m_get.Bind( self );
		m_set.Bind( self );
	}
}