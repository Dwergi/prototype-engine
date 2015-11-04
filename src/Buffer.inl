//
// Buffer.inl - A wrapper around a buffer. Just holds a pointer and a size.
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

template <typename T>
Buffer<T>::Buffer()
	: m_ptr( nullptr ),
	m_size( 0 )
{

}

template <typename T>
Buffer<T>::Buffer( T* ptr, uint size )
	: m_ptr( ptr ),
	m_size( size )
{
	
}

template <typename T>
Buffer<T>::Buffer( const Buffer<T>& other )
{
	Set( other.m_ptr, other.m_size );
}

template <typename T>
Buffer<T>::Buffer( Buffer<T>&& other )
{
	Set( other.m_ptr, other.m_size );

	other.m_ptr = nullptr;
	other.m_size = 0;
}

template <typename T>
Buffer<T>::~Buffer()
{
	Set( nullptr, 0 );
}

template <typename T>
Buffer<T>& Buffer<T>::operator=( const Buffer<T>& other )
{
	Set( other.m_ptr, other.m_size );

	return *this;
}

template <typename T>
void Buffer<T>::Set( T* ptr, uint size )
{
	ASSERT( m_ptr == nullptr, "Overwriting a Buffer pointer! Call Release first." );

	m_ptr = ptr;
	m_size = size;
}

template <typename T>
T* Buffer<T>::Get() const
{
	return m_ptr;
}

template <typename T>
Buffer<T>::operator T*() const
{
	return m_ptr;
}

template <typename T>
T* Buffer<T>::Release()
{
	T* ptr = m_ptr;

	m_ptr = nullptr;
	m_size = 0;

	return ptr;
}

template <typename T>
T& Buffer<T>::operator[]( uint index ) const
{
	ASSERT( index < m_size );

	return m_ptr[ index ];
}

template <typename T>
bool Buffer<T>::operator==( const Buffer<T>& other ) const
{
	return m_ptr == other.m_ptr && m_size == other.m_size;
}

template <typename T>
bool Buffer<T>::operator!=( const Buffer<T>& other ) const
{
	return !(*this == other);
}

template <typename T>
uint Buffer<T>::Size() const
{
	return m_size;
}
