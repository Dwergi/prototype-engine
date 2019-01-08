//
// SharedString.h - A shared string implementation where copies of the same string are refcounted and the underlying memory is freed only when none use it anymore.
// This string is intended to not be editable, because otherwise all copies of a string would be modified at the same time.
// Copyright (C) Sebastian Nordgren 
// November 5th 2015
//

#pragma once

#include "Iteration.h"

namespace dd
{
	class SharedString
	{
	public:

		SharedString();
		explicit SharedString( const char* str );
		explicit SharedString( const String& str );

		SharedString( const SharedString& other );
		SharedString( SharedString&& other );
		~SharedString();

		SharedString& operator=( const String& other );
		SharedString& operator=( const SharedString& other );
		SharedString& operator=( const char* other );

		const char* c_str() const {	return m_ptr.get(); }

		uint Length() const { return m_length; }
		uint UseCount() const { return m_ptr.use_count() - 1; } // take one away for the detail of the internal map

		bool operator==( const SharedString& other ) const;
		bool operator==( const char* other ) const;

		int Find( const SharedString& other, uint offset = 0 ) const;
		int Find( const char* other, uint offset = 0 ) const;

		char operator[]( uint index ) const;

		void Clear();

		DD_BASIC_TYPE( SharedString )
		
		DEFINE_ITERATORS( const char, *m_ptr, m_length )

	private:

		std::shared_ptr<const char> m_ptr;
		uint m_length { 0 };
		uint64 m_hash { 0 };

		static std::mutex s_mutex;
		static DenseMap<uint64, std::shared_ptr<const char>> s_instances;

		void Assign( const char* str );
	};
}

namespace std
{
	template <>
	struct hash<dd::SharedString>
	{
		size_t operator()( const dd::SharedString& str ) const
		{
			return (size_t) dd::HashString( str.c_str(), str.Length() );
		}
	};
}