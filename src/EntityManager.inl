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
		pool->GetWrite().Remove( handle );
	}

	template <typename Component>
	ComponentHandle<Component> EntityManager::AddComponent( EntityHandle handle ) const
	{
		if( handle.m_manager != this )
			return ComponentHandle<Component>();

		const DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();
		pool->GetWrite().Create( handle );
		return ComponentHandle<Component>( handle );
	}

	template <typename Component, typename... Args>
	ComponentHandle<Component> EntityManager::ConstructComponent( EntityHandle handle, Args&&... args ) const
	{
		if( handle.m_manager != this )
			return ComponentHandle<Component>();

		const DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();
		pool.GetWrite().Construct( handle, std::forward( args )... );
		return pool( handle, pool );
	}

	template <typename... Components>
	EntityHandle EntityManager::CreateEntity()
	{
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
		return pool->GetRead().Exists( handle );
	}

	template <typename Component>
	bool EntityManager::HasWritable( EntityHandle handle ) const
	{
		if( handle.m_manager != this )
			return false;

		const DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();
		return pool->GetWrite().Exists( handle );
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
		ExpandType
		{
			0, (CreateComponent<Components, Index>( handle ), 0)...
		};
	}

	template <typename Component, std::size_t Index>
	void EntityManager::CreateComponent( EntityHandle handle )
	{
		const DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();
		pool->GetWrite().Create( handle );
	}

	template <typename... Components>
	Vector<EntityHandle> EntityManager::FindAllWithReadable() const
	{
		Vector<EntityHandle> result;
		for( EntityHandle e : m_entities.GetRead() )
		{
			if( HasAllReadable<Components...>( e ) )
			{
				result.Add( e );
			}
		}

		return result;
	}

	template <typename... Components>
	Vector<EntityHandle> EntityManager::FindAllWithWritable() const
	{
		Vector<EntityHandle> result;
		for( EntityHandle e : m_entities.GetWrite() )
		{
			if( HasAllWritable<Components...>( e ) )
			{
				result.Add( e );
			}
		}

		return result;
	}

	template <typename... Components>
	void EntityManager::ForAllWithReadable( typename identity<std::function<void(EntityHandle, ComponentHandle<Components>...)>>::type f ) const
	{
		Vector<EntityHandle> result;
		for( EntityHandle e : m_entities.GetRead() )
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
		Vector<EntityHandle> result;
		for( EntityHandle e : m_entities.GetWrite() )
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
		return pool->GetRead().Find( h );
	}

	template <typename Component>
	Component* EntityManager::GetWritable( EntityHandle h ) const
	{
		DoubleBuffer<typename Component::Pool>* pool = GetPool<Component>();

		DD_ASSERT( pool != nullptr, "No pool found for component!" );
		return pool->GetWrite().Find( h );
	}

	template <typename Component>
	void EntityManager::RegisterComponent()
	{
		DoubleBuffer<typename Component::Pool>* double_buffer = new DoubleBuffer<typename Component::Pool>( new typename Component::Pool(), new typename Component::Pool() );
		
		const TypeInfo* typeInfo = TypeInfo::GetType<Component>();
		uint64 key = reinterpret_cast<uint64>(typeInfo);
		m_pools.Add( key, double_buffer );
	}

	template <typename Component>
	DoubleBuffer<typename Component::Pool>* EntityManager::GetPool() const
	{
		const TypeInfo* typeInfo = TypeInfo::GetType<Component>();
		uint64 key = reinterpret_cast<uint64>(typeInfo);

		DoubleBuffer<typename Component::Pool>* pool = reinterpret_cast<DoubleBuffer<typename Component::Pool>*>( *m_pools.Find( key ) );
		return pool;
	}
}