//
// HandleManager.inl - A generic, simple handle system that stores instances and uses indices to reference them.
// Copyright (C) Sebastian Nordgren 
// November 22nd 2018
//

namespace dd
{
	template <typename T>
	HandleManager<T>::HandleManager()
	{
		DD_ASSERT(Handle<T>::m_manager == nullptr);
		Handle<T>::m_manager = this;
	}

	template <typename T>
	Handle<T> HandleManager<T>::FindInternal( std::string name ) const
	{
		DD_ASSERT( !name.empty() );

		for( size_t i = 0; i < m_entries.size(); ++i )
		{
			if( (m_entries[i].Alive || m_entries[i].Create) &&
				m_entries[i].Instance->Name() == name )
			{
				return m_entries[i].Handle;
			}
		}

		return Handle<T>();
	}

	template <typename T>
	Handle<T> HandleManager<T>::Find( std::string name ) const
	{
		return FindInternal( name );
	}

	template <typename T>
	Handle<T> HandleManager<T>::Create( std::string name )
	{
		std::scoped_lock<std::mutex> lock( m_mutex );
		Handle<T> h = FindInternal( name );
		if( h.IsValid() )
		{
			return h;
		}

		if( m_free.empty() )
		{
			m_free.push_back( m_entries.size() );

			HandleEntry& entry = m_entries.emplace_back();
			entry.Handle.ID = m_entries.size() - 1;
			entry.Handle.Version = -1;
		}

		size_t idx = dd::pop_front( m_free );

		HandleEntry& entry = m_entries[idx];
		entry.Handle.Version++;
		entry.Create = true;

		DD_ASSERT( entry.Instance == nullptr );

		entry.Instance = new T();
		entry.Instance->SetName( name );

		return entry.Handle;
	}

	template <typename T>
	bool HandleManager<T>::IsAlive( Handle<T> h ) const
	{
		if( !h.IsValid() )
		{
			return false;
		}

		DD_ASSERT( h.ID >= 0 && h.ID < m_entries.size() );

		const HandleEntry& entry = m_entries[h.ID];

		return entry.Handle.Version == h.Version &&
			(entry.Alive || entry.Create);
	}

	template <typename T>
	void HandleManager<T>::Destroy( Handle<T> h )
	{
		DD_ASSERT( IsAlive( h ) );
		m_entries[h.ID].Destroy = true;
	}

	template <typename T>
	T* HandleManager<T>::Access( Handle<T> handle ) const
	{
		DD_ASSERT( IsAlive( handle ) );

		if( handle.ID < m_entries.size() )
		{
			return m_entries[handle.ID].Instance;
		}

		return nullptr;
	}

	template <typename T>
	const T* HandleManager<T>::Get( Handle<T> handle ) const
	{
		DD_ASSERT( IsAlive( handle ) );

		if( handle.ID < m_entries.size() )
		{
			return m_entries[handle.ID].Instance;
		}

		return nullptr;
	}

	template <typename T>
	size_t HandleManager<T>::Count() const
	{
		size_t count = 0;

		for( const HandleEntry& e : m_entries )
		{
			if( e.Alive )
			{
				++count;
			}
		}

		return count;
	}

	template <typename T>
	T* HandleManager<T>::AccessAt( size_t i ) const
	{
		size_t current = 0;
		for( const HandleEntry& e : m_entries )
		{
			if( e.Alive )
			{
				if( current == i )
				{
					return e.Instance;
				}

				++current;
			}
		}

		return nullptr;
	}

	template <typename T>
	void HandleManager<T>::Update()
	{
		for( HandleEntry& entry : m_entries )
		{
			if( entry.Destroy )
			{
				DD_ASSERT( entry.Instance != nullptr );

				OnDestroy( *entry.Instance );

				delete entry.Instance;
				entry.Instance = nullptr;

				entry.Alive = false;
				entry.Create = false;
				entry.Destroy = false;
			}

			if( entry.Create )
			{
				DD_ASSERT( entry.Instance != nullptr );

				OnCreate( *entry.Instance );

				entry.Alive = true;
				entry.Create = false;
				entry.Destroy = false;
			}
		}
	}
}