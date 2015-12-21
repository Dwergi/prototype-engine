#pragma once

#define DEFINE_ITERATORS( Type, Buffer, Size ) \
	typedef Iterator<Type> iterator; \
	Iterator<Type> begin() const { return dd::Iterator<Type>( (Type*) Buffer ); } \
	Iterator<Type> end() const { return dd::Iterator<Type>( (Type*) Buffer + Size ); }	

namespace dd
{
	template <typename T>
	class Iterator : public std::iterator<std::random_access_iterator_tag, T>
	{
	public:
		Iterator();
		explicit Iterator( T* data );

		T& operator*() const;
		T* operator->() const;
		Iterator& operator++();
		Iterator& operator--();
		Iterator operator++( int );
		Iterator operator--( int );
		Iterator operator+( size_t x ) const;
		Iterator operator-( size_t x ) const;
		size_t operator-( const Iterator& rhs ) const;
		Iterator& operator+=( size_t x );
		Iterator& operator-=( size_t x );
		bool operator<( const Iterator& rhs ) const;

		bool operator==( const Iterator &rhs ) const;
		bool operator!=( const Iterator &rhs ) const;

	private:
		T* m_data;
	};

	template <typename T>
	Iterator<T>::Iterator()
		: m_data( NULL )
	{
	}

	template <typename T>
	Iterator<T>::Iterator( T* data )
		: m_data( data )
	{
	}

	template <typename T>
	T& Iterator<T>::operator*() const
	{
		return *m_data;
	}

	template <typename T>
	T *Iterator<T>::operator->() const
	{
		return m_data;
	}

	template <typename T>
	typename Iterator<T>& Iterator<T>::operator++()
	{
		++m_data;
		return *this;
	}

	template <typename T>
	typename Iterator<T>& Iterator<T>::operator--()
	{
		--m_data;
		return *this;
	}

	template <typename T>
	typename Iterator<T> Iterator<T>::operator++( int )
	{
		return Iterator( m_data++ );
	}

	template <typename T>
	typename Iterator<T> Iterator<T>::operator--( int )
	{
		return Iterator( m_data-- );
	}

	template <typename T>
	bool Iterator<T>::operator==( const Iterator &rhs ) const
	{
		return m_data == rhs.m_data;
	}

	template <typename T>
	bool Iterator<T>::operator!=( const Iterator &rhs ) const
	{
		return m_data != rhs.m_data;
	}

	template <typename T>
	typename Iterator<T> Iterator<T>::operator+( size_t x ) const
	{
		return Iterator( m_data + x );
	}

	template <typename T>
	typename Iterator<T> Iterator<T>::operator-( size_t x ) const
	{
		return Iterator( m_data - x );
	}

	template <typename T>
	size_t Iterator<T>::operator-( const Iterator& rhs ) const
	{
		return m_data - rhs.m_data;
	}

	template <typename T>
	typename Iterator<T>& Iterator<T>::operator+=( size_t x )
	{
		m_data += x;
		return *this;
	}

	template <typename T>
	typename Iterator<T>& Iterator<T>::operator-=( size_t x )
	{
		m_data -= x;
		return *this;
	}

	template <typename T>
	bool Iterator<T>::operator<( const Iterator& rhs ) const
	{
		return m_data < rhs.m_data;
	}
}