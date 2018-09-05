//
// Buffer.inl - A wrapper around a buffer. Just holds a pointer and a size.
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

//
// ConstBuffer<T>
//
template <typename T>
ConstBuffer<T>::ConstBuffer() :
	IBuffer( sizeof( T ) )
{

}

template <typename T>
ConstBuffer<T>::ConstBuffer( const IBuffer& buffer ) :
	IBuffer( sizeof( T ) )
{
	m_ptr = (T*) buffer.GetVoid();
	m_count = buffer.SizeBytes() / m_elementSize;
}

template <typename T>
ConstBuffer<T>::ConstBuffer( const T* ptr, int count ) :
	IBuffer( sizeof( T ) )
{
	m_ptr = ptr;
	m_count = count;
}

template <typename T>
ConstBuffer<T>::ConstBuffer( const T* ptr, size_t count ) :
	IBuffer( sizeof( T ) )
{
	m_ptr = ptr;
	m_count = (int) count;
}

template <typename T>
ConstBuffer<T>::ConstBuffer( const ConstBuffer<T>& other ) :
	IBuffer( sizeof( T ) )
{
	Set( other.GetConst(), other.Size() );
}

template <typename T>
ConstBuffer<T>::ConstBuffer( ConstBuffer<T>&& other ) :
	IBuffer( sizeof( T ) )
{
	Set( other.GetConst(), other.Size() );

	other.m_ptr = nullptr;
	other.m_count = 0;
}

template <typename T>
ConstBuffer<T>::~ConstBuffer()
{
}

template <typename T>
ConstBuffer<T>& ConstBuffer<T>::operator=( const ConstBuffer<T>& other )
{
	Set( other.GetConst(), other.Size() );

	return *this;
}

template <typename T>
ConstBuffer<T>& ConstBuffer<T>::operator=( ConstBuffer<T>&& other )
{
	Set( other.GetConst(), other.Size() );
	other.ReleaseConst();

	return *this;
}

template <typename T>
void ConstBuffer<T>::Set( const T* ptr, int count )
{
	DD_ASSERT( m_ptr == nullptr, "Overwriting a ConstBuffer pointer! Call ReleaseConst first." );

	m_ptr = ptr;
	m_count = count;
}

template <typename T>
const T* ConstBuffer<T>::GetConst() const
{
	return static_cast<const T*>(m_ptr);
}

template <typename T>
const T* ConstBuffer<T>::ReleaseConst()
{
	const T* ptr = GetConst();

	m_ptr = nullptr;
	m_count = 0;

	return ptr;
}

template <typename T>
void ConstBuffer<T>::Delete()
{
	const T* ptr = ReleaseConst();

	if( ptr != nullptr )
	{
		delete[] ptr;
	}
}

template <typename T>
int ConstBuffer<T>::Size() const
{
	return m_count;
}

template <typename T>
const T& ConstBuffer<T>::operator[]( size_t index ) const
{
	DD_ASSERT( index < m_count );

	return GetConst()[ index ];
}

template <typename T>
bool ConstBuffer<T>::operator==( const ConstBuffer<T>& other ) const
{
	return m_ptr == other.m_ptr && m_count == other.m_count;
}

template <typename T>
bool ConstBuffer<T>::operator!=( const ConstBuffer<T>& other ) const
{
	return !(*this == other);
}

//
// Buffer<T>
//
template <typename T>
Buffer<T>::Buffer()
{

}

template <typename T>
Buffer<T>::Buffer( T* ptr, int count ) :
	ConstBuffer( ptr, count )
{
	
}

template <typename T>
Buffer<T>::Buffer( T* ptr, size_t count ) :
	ConstBuffer( ptr, count )
{

}

template <typename T>
Buffer<T>::Buffer( const Buffer<T>& other )
{
	Set( other.Get(), other.Size() );
}

template <typename T>
Buffer<T>::Buffer( Buffer<T>&& other )
{
	Set( other.Get(), other.Size() );

	other.m_ptr = nullptr;
	other.m_count = 0;
}

template <typename T>
Buffer<T>::~Buffer()
{
}

template <typename T>
Buffer<T>& Buffer<T>::operator=( const Buffer<T>& other )
{
	Set( other.Get(), other.Size() );

	return *this;
}

template <typename T>
Buffer<T>& Buffer<T>::operator=( Buffer<T>&& other )
{
	Set( other.Get(), other.Size() );
	other.Release();

	return *this;
}

template <typename T>
void Buffer<T>::Set( T* ptr, int count )
{
	DD_ASSERT( m_ptr == nullptr, "Overwriting a Buffer pointer! Call Release first." );

	m_ptr = ptr;
	m_count = count;
}

template <typename T>
T* Buffer<T>::Get() const
{
	return (T*) m_ptr;
}

template <typename T>
T* Buffer<T>::Release()
{
	T* ptr = Get();

	m_ptr = nullptr;
	m_count = 0;

	return ptr;
}

template <typename T>
T& Buffer<T>::operator[]( size_t index ) const
{
	DD_ASSERT( index < m_count );

	return Get()[ index ];
}


template <typename T>
bool Buffer<T>::operator==( const Buffer<T>& other ) const
{
	return m_ptr == other.m_ptr && m_count == other.m_count;
}

template <typename T>
bool Buffer<T>::operator!=( const Buffer<T>& other ) const
{
	return !(*this == other);
}