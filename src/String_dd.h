//
// String_dd.h - String implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	class StringBase
	{
	private:
		void Initialize();

		typedef unsigned int uint;

	protected:
		StringBase();
		StringBase( uint size );
		StringBase( const char* other );
		StringBase( const char* other, uint size );
		StringBase( StringBase&& other );
		StringBase( const StringBase& other );

		~StringBase();

	public:

		StringBase& operator=( const StringBase& other );
		bool operator==( const StringBase& other ) const;
		StringBase& operator+=( const StringBase& other );

		int Find( const StringBase& other ) const;
		StringBase& Substring( uint start, uint end );

		void Clear() { m_length = 0; }

		const char& operator[]( uint index ) { ASSERT( index >= 0 && index < m_length ); return m_buffer[ index ]; }
		const char* c_str() const;

		uint Length() const { return m_length; }
		bool IsEmpty() const { return m_length == 0; }

		//
		// Iteration
		//
		class const_iterator
		{
		public:
			const char* Pointer;

			const_iterator() : Pointer( nullptr ) {}
			const_iterator( const char* ptr ) : Pointer( ptr ) {}

			inline const char& operator*() const { return *Pointer; }
			inline const_iterator& operator++() { ++Pointer; return *this; }
			inline const_iterator& operator+( size_t count ) { Pointer += count; return *this; }
			inline bool operator!=( const const_iterator& other ) const { return Pointer != other.Pointer; }
		};

		class iterator : public const_iterator
		{
		public:
			iterator() : const_iterator() {}
			iterator( char* ptr ) : const_iterator( ptr ) {}

			inline char& operator*() const { return *const_cast<char*>( Pointer ); }
		};

		inline const_iterator begin() const { return const_iterator( m_buffer ); }
		inline const_iterator end() const { return const_iterator( m_buffer + m_length ); }

		inline iterator begin() { return iterator( m_buffer ); }
		inline iterator end() { return iterator( m_buffer + m_length ); }

	protected:

		uint m_length;
		char* m_buffer;

		uint m_capacity;
		uint m_stackCapacity;
		char* m_stackBuffer;

		void Resize( uint length );

	private:

		void SetString( const char* data, uint length );
	};


	template< int Size >
	class String
		: public StringBase
	{
	public:

		String()
		{
			m_stackBuffer = m_stackData;
			m_stackCapacity = Size;
			m_capacity = m_stackCapacity;
		}

		String( const StringBase& other )
		{
			m_stackBuffer = m_stackData;
			m_stackCapacity = Size;
			m_capacity = m_stackCapacity;

			*this = other;
		}

	private:
		char m_stackData[ Size ];
	};
}