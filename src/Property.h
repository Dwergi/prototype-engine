//
// Property.h - A wrapper around a member that can be used to create a property editor.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#pragma once

#define PTR_OFFSET( Pointer, Bytes ) ((char*) (Pointer) + (Bytes))

namespace dd
{
	class MemberBase;

	class Property
	{
	private:
		// the raw data pointer
		void* m_data;

		// used for nested classes, where the TypeInfo's given pointer needs to be shifted to compensate for earlier members it doesn't know about
		uint m_offset;

		// the member this property is bound to
		MemberBase* m_member;

	public:
		Property( MemberBase* member, uint offset );
		Property( Property&& entry );
		~Property();

		template< typename T >
		void Get( T& value )
		{
			ASSERT( m_data != nullptr, "Must bind property before getting its value!" );

			value = *reinterpret_cast<T*>( m_data );
		}

		template< typename T >
		void Set( const T& value )
		{
			ASSERT( m_data != nullptr, "Must bind property before setting its value!" );

			*(reinterpret_cast<T*>( m_data )) = value;
		}

		template< typename T >
		void Bind( T& instance )
		{
			m_data = PTR_OFFSET( m_member->BindProperty( &instance ), m_offset );
		}

		template< typename T >
		T* GetPtr() const
		{
			ASSERT( m_data != nullptr, "Must bind property before getting a reference to its data." );

			return reinterpret_cast<T*>( m_data );
		}

		const std::string& GetName() const;
		const std::string& GetDisplayName() const;
	}; 
}