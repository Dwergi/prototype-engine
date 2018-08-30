//
// EntityManager.inl - System that handles the batched creation/deletion of entities and stores all entities.
// Copyright (C) Sebastian Nordgren 
// December 19th 2016
//

namespace dd
{
	template <typename Component>
	void EntityManager::RemoveComponent( EntityHandle handle ) const
	{
		if( handle.m_manager != this )
			return;

		const DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();
		pool->Write().Remove( handle );
	}

	template <typename Component>
	ComponentHandle<Component> EntityManager::AddComponent( EntityHandle handle ) const
	{
		if( handle.m_manager != this )
			return ComponentHandle<Component>();

		const DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();
		pool->Write().Create( handle );
		return ComponentHandle<Component>( handle );
	}

	template <typename Component, typename... Args>
	ComponentHandle<Component> EntityManager::ConstructComponent( EntityHandle handle, Args&&... args ) const
	{
		if( handle.m_manager != this )
			return ComponentHandle<Component>();

		const DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();
		pool.Write().Construct( handle, std::forward( args )... );
		return pool( handle, pool );
	}

	template <typename... Components>
	EntityHandle EntityManager::CreateEntity()
	{
		std::lock_guard<std::recursive_mutex> lock( m_mutex );

		EntityHandle handle = Create();
		CreateComponents<Components...>( handle, std::make_index_sequence<sizeof...(Components)>() );
		return handle;
	}

	template <typename Component>
	ComponentHandle<Component> EntityManager::GetComponent( EntityHandle handle ) const
	{
		if( handle.m_manager == this && handle.IsValid() && handle.Has<Component>() )
		{
			return ComponentHandle<Component>( handle );
		}

		return ComponentHandle<Component>();
	}

	template <typename Component>
	bool EntityManager::HasComponent( EntityHandle handle ) const
	{
		return HasReadable<Component>( handle ) || HasWritable<Component>( handle );
	}

	template <typename Component>
	bool EntityManager::HasReadable( EntityHandle handle ) const
	{
		if( handle.m_manager != this )
			return false;

		const DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();
		return pool->Read().Exists( handle );
	}

	template <typename Component>
	bool EntityManager::HasWritable( EntityHandle handle ) const
	{
		if( handle.m_manager != this )
			return false;

		const DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();
		return pool->Write().Exists( handle );
	}

	template <typename... Components>
	bool EntityManager::HasAllReadable( EntityHandle handle ) const
	{
		bool exists[] =
		{
			HasReadable<Components>( handle )...
		};

		for( bool b : exists )
		{
			if( b == false )
			{
				return false;
			}
		}

		return true;
	}

	template <typename... Components>
	bool EntityManager::HasAllWritable( EntityHandle handle ) const
	{
		bool exists[] =
		{
			HasWritable<Components>( handle )...
		};

		for( bool b : exists )
		{
			if( b == false )
			{
				return false;
			}
		}

		return true;
	}

	template <typename... Components, std::size_t... Index>
	void EntityManager::CreateComponents( EntityHandle handle, std::index_sequence<Index...> )
	{
		std::lock_guard<std::recursive_mutex> lock( m_mutex );

		ExpandType
		{
			0, (CreateComponent<Components, Index>( handle ), 0)...
		};
	}

	template <typename Component, std::size_t Index>
	void EntityManager::CreateComponent( EntityHandle handle )
	{
		std::lock_guard<std::recursive_mutex> lock( m_mutex );

		const DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();
		pool->Write().Create( handle );
	}

	template <typename... Components>
	std::vector<EntityHandle> EntityManager::FindAllWithReadable() const
	{
		std::vector<EntityHandle> result;
		for( EntityHandle e : m_entities.Read() )
		{
			if( HasAllReadable<Components...>( e ) )
			{
				result.push_back( e );
			}
		}

		return result;
	}

	template <typename... Components>
	std::vector<EntityHandle> EntityManager::FindAllWithWritable() const
	{
		std::vector<EntityHandle> result;
		for( EntityHandle e : m_entities.Write() )
		{
			if( HasAllWritable<Components...>( e ) )
			{
				result.push_back( e );
			}
		}

		return result;
	}

	template <typename... Components>
	void EntityManager::ForAllWithReadable( typename identity<std::function<void(EntityHandle, ComponentHandle<Components>...)>>::type f ) const
	{
		for( EntityHandle e : m_entities.Read() )
		{
			if( HasAllReadable<Components...>( e ) )
			{
				f( e, GetComponent<Components>( e )... );
			}
		}
	}

	template <typename... Components>
	void EntityManager::ForAllWithWritable( typename identity<std::function<void( EntityHandle, ComponentHandle<Components>... )>>::type f ) const
	{
		for( EntityHandle e : m_entities.Write() )
		{
			if( HasAllWritable<Components...>( e ) )
			{
				f( e, GetComponent<Components>( e )... );
			}
		}
	}

	template <typename Component>
	const Component* EntityManager::GetReadable( EntityHandle h ) const
	{
		DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();

		DD_ASSERT( pool != nullptr, "No pool found for component!" );
		return pool->Read().Find( h );
	}

	template <typename Component>
	Component* EntityManager::GetWritable( EntityHandle h ) const
	{
		DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();

		DD_ASSERT( pool != nullptr, "No pool found for component!" );
		return pool->Write().Find( h );
	}

	template <typename Component>
	void EntityManager::RegisterComponent()
	{
		std::lock_guard<std::recursive_mutex> lock( m_mutex );

		const TypeInfo* typeInfo = TypeInfo::GetType<Component>();

		DoubleBuffer<typename Component::Pool>* doubleBuffer = new DoubleBuffer<typename Component::Pool>( new typename Component::Pool(), new typename Component::Pool() );

		uint64 key = reinterpret_cast<uint64>(typeInfo);
		m_pools.Add( key, doubleBuffer );
	}

	template <typename Component>
	DoubleBuffer<typename Component::Pool>* EntityManager::GetPool() const
	{
		const TypeInfo* typeInfo = TypeInfo::GetType<Component>();
		uint64 key = reinterpret_cast<uint64>(typeInfo);

		DoubleBuffer<typename Component::Pool>* pool = reinterpret_cast<DoubleBuffer<typename Component::Pool>*>( *m_pools.Find( key ) );
		DD_ASSERT( pool != nullptr, "No pool found for component!" );

		return pool;
	}
}