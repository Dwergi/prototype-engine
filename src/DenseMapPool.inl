//
// DenseMapPool.inl - Component pool that uses an unordered map for storage.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

namespace dd
{
	template<typename T>
	class DenseMapPoolIterator
	{
	private:

		DenseMapIterator<int, T> Iter;

	public:

		DenseMapPoolIterator( DenseMapIterator<int, T>& it )
			: Iter( it ) {}

		DenseMapPoolIterator( const DenseMapPoolIterator<T>& other )
			: Iter( other.Iter ) {}

		const T& operator*() const
		{
			return (*Iter).Value;
		}

		T& operator*()
		{
			return (*Iter).Value;
		}

		DenseMapPoolIterator<T> operator++()
		{
			++Iter;
			return *this;
		}

		bool operator!=( const DenseMapPoolIterator<T>& other ) const
		{
			return Iter != other.Iter;
		}
	};

	template <typename T>
	DenseMapPool<T>::DenseMapPool()
	{

	}

	template <typename T>
	DenseMapPool<T>::DenseMapPool( DenseMapPool<T>&& other )
		: m_components( std::move( other.m_components ) )
	{

	}

	template <typename T>
	DenseMapPool<T>::DenseMapPool( const DenseMapPool<T>& other )
		: m_components( other.m_components )
	{

	}

	template <typename T>
	DenseMapPool<T>::~DenseMapPool()
	{
		Clear();
	}

	template <typename T>
	DenseMapPool<T>& DenseMapPool<T>::operator=( DenseMapPool<T>&& other )
	{
		m_components = std::move( other.m_components );

		return *this;
	}

	template <typename T>
	DenseMapPool<T>& DenseMapPool<T>::operator=( const DenseMapPool<T>& other )
	{
		m_components = other.m_components;

		return *this;
	}

	template <typename T>
	void DenseMapPool<T>::Clear()
	{
		m_components.Clear();
	}

	template <typename T>
	uint DenseMapPool<T>::Size() const
	{
		return m_components.Size();
	}

	template <typename T>
	T* DenseMapPool<T>::Create( const EntityHandle& entity )
	{
		// already allocated!
		T* pCmp = Find( entity );
		if( pCmp != nullptr )
		{
			DD_ASSERT( false, "Component already allocated for given entity!" );
			return nullptr;
		}

		m_components.Add( entity.ID, T() );
		return &m_components[ entity.ID ];
	}

	template <typename T>
	T* DenseMapPool<T>::Find( const EntityHandle& entity ) const
	{
		T* value = m_components.Find( entity.ID );
		return value;
	}

	template <typename T>
	void DenseMapPool<T>::Remove( const EntityHandle& entity )
	{
		m_components.Remove( entity.ID );
	}

	template <typename T>
	bool DenseMapPool<T>::Exists( const EntityHandle& entity ) const
	{
		T* cmp = Find( entity );

		return cmp != nullptr;
	}

	template <typename T>
	typename DenseMapPoolIterator<T> DenseMapPool<T>::begin() const
	{
		return DenseMapPoolIterator<T>( m_components.begin() );
	}

	template <typename T>
	typename DenseMapPoolIterator<T> DenseMapPool<T>::end() const
	{
		return DenseMapPoolIterator<T>( m_components.end() );
	}
}