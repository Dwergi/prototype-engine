//
// DenseMapPool.inl - Component pool that uses an unordered map for storage.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

namespace dd
{
	template <typename T>
	class DenseMapPoolIterator
	{
	private:

		typename std::unordered_map<int, T>::iterator Iter;

	public:

		DenseMapPoolIterator( typename std::unordered_map<int, T>::iterator& it )
			: Iter( it )
		{
		}

		DenseMapPoolIterator( const DenseMapPoolIterator<T>& other )
			: Iter( other.Iter ) 
		{
		}

		const T& operator*() const
		{
			return (*Iter).second;
		}

		T& operator*()
		{
			return (*Iter).second;
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
		m_components.clear();
	}

	template <typename T>
	uint DenseMapPool<T>::Size() const
	{
		return m_components.Size();
	}

	template <typename T>
	T* DenseMapPool<T>::Create( EntityHandle entity )
	{
		// already allocated!
		T* pCmp = Find( entity );
		if( pCmp != nullptr )
		{
			DD_ASSERT( false, "Component already allocated for given entity!" );
			return nullptr;
		}

		m_components.insert( std::make_pair<int, T>( entity.ID, T() ) );
		T* cmp = &m_components[ entity.ID ];

		IComponent* baseptr = static_cast<IComponent*>(cmp);
		baseptr->Entity = entity;

		return cmp;
	}

	template <typename T>
	T* DenseMapPool<T>::Find( EntityHandle entity ) const
	{
		auto it = m_components.find( entity.ID );
		if( it == m_components.end() )
			return nullptr;

		return const_cast<T*>( &it->second );
	}

	template <typename T>
	void DenseMapPool<T>::Remove( EntityHandle entity )
	{
		m_components.erase( entity.ID );
	}

	template <typename T>
	bool DenseMapPool<T>::Exists( EntityHandle entity ) const
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