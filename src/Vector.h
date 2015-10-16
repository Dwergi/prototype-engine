//
// Vector.h - A vector.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	class VectorBase
	{
	public:
		VectorBase();
		uint Size() const;
		uint Capacity() const;

	protected:
		uint m_capacity;
		uint m_size;

		static const uint DefaultSize = 8;

		// growth factor and growth fudge defines 
		static const float GrowthFactor;
		static const uint GrowthFudge;
	};

	template< typename T >
	class Vector : public VectorBase
	{
	public:

		Vector();
		Vector( uint reserved_size );
		Vector( const Vector& other );
		Vector( Vector&& other );

		Vector<T>& operator=( const Vector& other );
		T& operator[]( uint index ) const;

		//
		// Remove from the given index in an unordered way. 
		// Swaps the last element with the given index.
		//
		void Remove( uint index );
		void Remove( int index );

		void RemoveItem( const T& item );

		//
		// Remove from the given index in an ordered way.
		// This will invalidate indices or pointers into the vector after the given index.
		//
		void RemoveOrdered( uint index );
		void RemoveAll( const Vector<T>& to_remove );
		void Clear();
		T Pop();

		//
		// Zero the entry by calling the destructor on it. 
		// Extremely unsafe, because this does not decrement the size of the container. 
		// You're basically on your own at this point.
		//
		void Zero( uint index ) const;

		void Add( T&& value );
		void Add( const T& value );
		void Push( T&& value );
		void Push( const T& value );

		//
		// Insert at the given index. 
		// Index must be <= Size()
		//
		void Insert( const T&& value, uint index );

		//
		// Add all the entries from the given vector to the end of this vector.
		//
		void AddAll( const Vector<T>& other );

		//
		// Create a new entry at the back and return a reference to it. 
		//
		T& Allocate();
		T& Allocate( T&& entry );

		void Swap( Vector<T>& other );

		int Find( const T& entry ) const;
		bool Contains( const T& entry ) const;

		//
		// Reserve at least capacity elements. 
		// Does NOT change the size of the container.
		// Does NOT destroy any elements, even if capacity is less than the current size.
		//
		void Reserve( uint capacity );

		//
		// Resize the vector to contain exactly size elements.
		// Will destroy any outside it and create any missing ones.
		//
		void Resize( uint size );

		void ShrinkToFit();

		bool operator==( const Vector<T>& other ) const;

		//
		// Iteration
		//
		DEFINE_ITERATORS( T, m_data, m_size )

	private:

		// 
		// Grows the vector to contain at least target elements.
		// The actual capacity will depend on the growth factor.
		//
		void Grow( uint capacity );
		void Grow();

		void Reallocate( uint new_capacity );

		T& GetEntry( uint index ) const;

		T* m_data;
	};
}

#include "Vector.inl"