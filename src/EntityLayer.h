#pragma once

#include "ComponentType.h"

#include <bitset>
#include <unordered_map>
#include <future>

namespace dd
{
	class JobSystem;
}

namespace ddc
{
	struct System;

	struct Entity
	{
		uint Alive : 1;
		uint ID : 21;
		uint Version : 10;
	};

	struct EntityLayer
	{
		EntityLayer();

		Entity Create();
		void Destroy( Entity entity );
		bool IsAlive( Entity entity ) const;

		void* AccessComponent( Entity entity, TypeID id ) const;

		template <typename T> 
		T* AccessComponent( Entity entity ) const
		{
			return reinterpret_cast<T*>( AccessComponent( entity, T::Type.ID ) );
		}

		const void* GetComponent( Entity entity, TypeID id ) const;

		template <typename T>
		const T* GetComponent( Entity entity ) const
		{
			return reinterpret_cast<const T*>(GetComponent( entity, T::Type.ID ));
		}

		bool HasComponent( Entity entity, TypeID id ) const;

		template <typename T>
		bool HasComponent( Entity entity ) const
		{
			return HasComponent( entity, T::Type.ID );
		}

		void* AddComponent( Entity entity, TypeID id );

		template <typename T>
		T& AddComponent( Entity entity )
		{
			return *reinterpret_cast<T*>( AddComponent( entity, T::Type.ID ) );
		}

		void RemoveComponent( Entity entity, TypeID id );

		template <typename T>
		void RemoveComponent( Entity entity )
		{
			RemoveComponent( entity, T::Type.ID );
		}

		void FindAllWith( const dd::IArray<int>& nodes, std::vector<Entity>& outEntities ) const;

	private:

		int m_count { 0 };

		std::vector<Entity> m_entities;
		std::vector<int> m_free;
		std::vector<std::bitset<MAX_COMPONENTS>> m_ownership;

		std::vector<byte*> m_components;
	};

	struct SystemNode
	{
		struct Edge
		{
			size_t m_from;
			size_t m_to;
		};

		System* m_system { nullptr };

		std::vector<Edge> m_out;
		std::vector<Edge> m_in;

		std::shared_future<void> m_update;
	};

	const int PARTITION_COUNT = 4;

	void UpdateSystem( System& system, EntityLayer& space, int partitions = PARTITION_COUNT );

	void OrderSystemsByComponent( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_nodes );

	void OrderSystemsByDependencies( dd::Span<System*> systems, std::vector<SystemNode>& out_ordered_nodes );

	void UpdateSystemsWithTreeScheduling( std::vector<SystemNode>& systems, dd::JobSystem& jobsystem, EntityLayer& layer );
}