//
// Vector.h - A vector.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	constexpr uint64 InvalidIndex = ~0ull;

	template <typename T>
	struct Vector
	{
		Vector();
		Vector(T* data, uint64 size, uint64 capacity, bool can_delete);
		Vector(const Vector& other);
		Vector(Vector&& other);

		Vector<T>& operator=(Vector&& other);
		Vector<T>& operator=(const Vector& other);

		T& operator[](uint64 index) const;

		uint64 Size() const;
		uint64 Capacity() const;

		//
		// Remove from the given index in an unordered way. 
		// Swaps the last element with the given index.
		//
		void Remove(uint64 index);

		//
		// Remove all instances that are equivalent to the given item from the container.
		//
		void RemoveItem(const T& item);

		//
		// Remove from the given index in an ordered way.
		// This will invalidate indices or pointers into the vector after the given index.
		//
		void RemoveOrdered(uint64 index);
		void RemoveAll(const Vector<T>& to_remove);
		void Clear();

		//
		// Remove and return the last entry.
		//
		T Pop();

		//
		// Zero the entry by calling the destructor on it. 
		// Extremely unsafe, because this does not decrement the size of the container. 
		// You're basically on your own at this point.
		//
		void Zero(uint64 index) const;

		//
		// Add the given value.
		// Push and Add are identical.
		//
		void Add(T&& value);
		void Add(const T& value);

		//
		// Insert at the given index. 
		// Index must be <= Size() and >= 0
		//
		void Insert(const T&& value, uint64 index);
		void Insert(const T& value, uint64 index);

		//
		// Add all the entries from the given vector to the end of this vector.
		//
		void AddAll(const Vector<T>& other);

		//
		// Create a new entry at the back and return a reference to it. 
		//
		T& Allocate();
		T& Allocate(T&& entry);

		void Swap(Vector<T>& other);

		//
		// Reverse the order of the elements in the vector.
		//
		void Reverse();

		uint64 Find(const T& entry) const;
		bool Contains(const T& entry) const;

		T* Data() const;

		//
		// Reserve at least capacity elements. 
		// Does NOT change the size of the container.
		// Does NOT destroy any elements, even if capacity is less than the current size.
		//
		void Reserve(uint64 capacity);

		//
		// Resize the vector to contain exactly size elements.
		// Will destroy any outside it and create any missing ones.
		//
		void Resize(uint64 size);

		void ShrinkToFit();

		bool operator==(const Vector<T>& other) const;

		//
		// Get the first and last entries in the vector.
		//
		T& First() const;
		T& Last() const;

		//
		// Iteration
		//
		DD_DEFINE_ITERATORS(T, m_data, m_size)

	private:

		// 
		// Grows the vector to contain at least target elements.
		// The actual capacity will depend on the growth factor.
		//
		void Grow(uint64 capacity);
		void Grow();

		void Reallocate(uint64 new_capacity);

		T& GetEntry(uint64 index) const;

		uint64 m_capacity { 0 };
		uint64 m_size { 0 };
		T* m_data { nullptr };
		bool m_deallocate { true };

		static const float GrowthFactor;
		static const uint64 GrowthFudge;
		static const uint64 DefaultSize;
	};

	template <typename T, size_t N = 64>
	struct StackVector : Vector<T>
	{
		StackVector() : Vector<T>((T*) m_stack, 0, N, false) {}

		template <size_t OtherSize>
		StackVector(const StackVector<T, OtherSize>& other)
			: Vector<T>((T*) m_stack, 0, N, false)
		{
			AddAll(other);
		}

		bool IsOnStack() const { return Vector<T>::Data() == (T*) m_stack; }

	private:
		byte m_stack[sizeof(T) * N];
	};
}

#include "Vector.inl"