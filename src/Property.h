//
// Property.h - A wrapper around a member that can be used to create a property editor.
// Copyright (C) Sebastian Nordgren 
// September 2nd 2015
// Significantly influenced by Randy Gaul (http://RandyGaul.net)
//

#pragma once

#include "Member.h"

namespace dd
{
	template <typename T>
	class Recorder;

	template <typename T>
	class FullRecorder;

	class Property
	{
	public:

		Property();
		Property( Member& self, void* ptr );
		Property( Property&& other );
		Property( const Property& other );
		~Property();

		// Get or set the member this property wraps
		template <typename T>
		void Get( T& ret );
		template <typename T>
		void Set( const T& val );

		const String& Name() { return m_member->Name(); }

	private:

		Member* m_member;
		void* m_ptr;

		void* GetPtr() const { return m_ptr; }

		template <typename T>
		friend class Recorder;

		template <typename T>
		friend class FullRecorder;
	};

	template <typename T>
	void Property::Get( T& out )
	{
		DD_ASSERT( m_ptr != nullptr );

		out = *reinterpret_cast<const T*>( m_ptr );
	}

	template <typename T>
	void Property::Set( const T& val )
	{
		DD_ASSERT( m_ptr != nullptr );

		*reinterpret_cast<T*>( m_ptr ) = val;
	}
}