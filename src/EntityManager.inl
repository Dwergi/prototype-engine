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
		return Services::GetWritePool<Component>().Remove( handle );
	}

	template <typename Component>
	Component* EntityManager::AddComponent( EntityHandle handle ) const
	{
		return Services::GetWritePool<Component>().Create( handle );
	}

	template <typename... Components>
	EntityHandle EntityManager::CreateEntity()
	{
		EntityHandle handle = Create();

		CreateComponents<Components...>( handle, std::make_index_sequence<sizeof...(Components)>() );

		return handle;
	}

	template <typename Component>
	Component& EntityManager::GetComponent( EntityHandle handle ) const
	{
		const Component::Pool& pool = Services::GetReadPool<Component>();
		return *pool.Find( handle );
	}

	template <typename Component>
	bool EntityManager::HasComponent( EntityHandle handle ) const
	{
		const Component::Pool& pool = Services::GetReadPool<Component>();
		return pool.Exists( handle );
	}

	template <typename... Components>
	bool EntityManager::HasAllComponents( EntityHandle handle ) const
	{
		bool exists[] =
		{
			HasComponent<Components>( handle )...
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
		Services::GetWritePool<Component>().Create( handle );
	}

	template <typename... Components>
	Vector<EntityHandle> EntityManager::FindAllWith() const
	{
		Vector<EntityHandle> result;
		for( EntityEntry e : m_entities )
		{
			if( HasAllComponents<Components...>( e.Entity ) )
			{
				result.Add( e.Entity );
			}
		}

		return result;
	}

	template <typename... Components>
	void EntityManager::ForAllWith( typename identity<std::function<void(EntityHandle, Components&...)>>::type f ) const
	{
		Vector<EntityHandle> result;
		for( EntityEntry e : m_entities )
		{
			if( HasAllComponents<Components...>( e.Entity ) )
			{
				f( e.Entity, GetComponent<Components>( e.Entity )... );
			}
		}
	}
}