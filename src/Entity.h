#pragma once

#include "Array.h"
#include "AutoList.h"
#include "Buffer.h"
#include "Span.h"

#include <bitset>
#include <unordered_map>

namespace ddc
{
#define DD_COMPONENT const ddc::ComponentType& GetType() const { return Type; } \
	static void Construct( void* ptr ); \
	static const ddc::ComponentType Type

#define DD_COMPONENT_CPP( TypeName ) DD_STATIC_ASSERT( std::is_trivially_copyable_v<TypeName> && std::is_default_constructible_v<TypeName> ); \
	void TypeName::Construct( void* ptr ) { new (ptr) TypeName; } \
	const ddc::ComponentType TypeName::Type( #TypeName, sizeof( TypeName ), &TypeName::Construct )

	static const int MAX_ENTITIES = 32 * 1024;
	static const int MAX_COMPONENTS = 32;

	struct EntityLayer;

	typedef int TypeID;

	struct ComponentType
	{
		static ComponentType* Types[ MAX_COMPONENTS ];
		static int Count;

		static const TypeID InvalidID = -1;

		TypeID ID;
		const char* Name;
		size_t Size;
		void (*Construct)( void* );

		ComponentType( const char* name, size_t size, void (*ctor)( void* ) )
		{
			Name = name;
			ID = Count;
			Size = size;
			Construct = ctor;

			Types[ ID ] = this;
			++Count;
		}
	};

	typedef int Entity;

	struct EntityLayer
	{
		EntityLayer();

		Entity Create();
		void Destroy( Entity entity );
		bool IsAlive( Entity entity );

		void* AccessComponent( Entity entity, TypeID id );

		template <typename T> 
		T* AccessComponent( Entity entity )
		{
			return reinterpret_cast<T*>( AccessComponent( entity, T::Type.ID ) );
		}

		bool HasComponent( Entity entity, TypeID id )
		{
			if( !IsAlive( entity ) )
			{
				return false;
			}

			return m_ownership[ entity ].test( id );
		}

		template <typename T>
		bool HasComponent( Entity entity )
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

		void FindAllWith( const dd::IArray<int>& nodes, std::vector<int>& outEntities );

	private:

		int m_count { 0 };

		std::bitset<MAX_ENTITIES> m_alive;
		std::vector<Entity> m_free;
		std::vector<std::bitset<MAX_COMPONENTS>> m_ownership;

		std::vector<void*> m_components;
	};

	enum class DataUsage
	{
		Invalid,
		Read,
		Write
	};

	struct DataRequirement
	{
		DataRequirement( const DataRequirement& other )
		{
			m_component = other.m_component;
			m_usage = other.m_usage;
			m_buffer = other.m_buffer;
		}

		~DataRequirement()
		{
			delete[] m_buffer;
		}

		DataUsage Usage() const { return m_usage; }
		TypeID Component() const { return m_component; }

		byte* GetBuffer() const { return m_buffer; }

	protected:
		DataRequirement( TypeID component, size_t component_size, DataUsage usage ) :
			m_component( component ),
			m_usage( usage )
		{
			m_buffer = new byte[ component_size * MAX_ENTITIES ];
		}

	private:
		TypeID m_component { 0 };
		DataUsage m_usage { DataUsage::Invalid };
		byte* m_buffer { nullptr };
	};

	struct UpdateData;

	struct System
	{
		System( const char* name ) : m_name( name ) {}

		void RegisterDependency( const System& system ) { DD_ASSERT( &system != this ); m_dependencies.Add( &system ); }

		void RegisterDataRequirement( const DataRequirement& req ) { m_requirements.Add( &req ); }

		const dd::IArray<const DataRequirement*>& GetRequirements() const { return m_requirements; }
		const dd::IArray<const System*>& GetDependencies() const { return m_dependencies; }

		virtual void Update( const UpdateData& data ) = 0;

	
	private:
		dd::Array<const DataRequirement*, MAX_COMPONENTS> m_requirements;
		dd::String64 m_name;
		dd::Array<const System*, 32> m_dependencies;
	};

	template <typename TComponent>
	struct ReadRequirement : DataRequirement
	{
		ReadRequirement( System& system ) :
			DataRequirement( TComponent::Type.ID, TComponent::Type.Size, DataUsage::Read )
		{
			system.RegisterDataRequirement( *this );
		}
	};

	template <typename TComponent>
	struct WriteRequirement : DataRequirement
	{
		WriteRequirement( System& system ) :
			DataRequirement( TComponent::Type.ID, TComponent::Type.Size, DataUsage::Write )
		{
			system.RegisterDataRequirement( *this );
		}
	};

	struct ComponentDataBuffer
	{
		ComponentDataBuffer( dd::Span<Entity> entities, EntityLayer& space, TypeID component, DataUsage usage, byte* storage ) :
			m_component( component ),
			m_usage( usage ),
			m_storage( storage )
		{
			DD_ASSERT( storage != nullptr );

			const ComponentType* type = ComponentType::Types[ m_component ];

			m_count = entities.Size();

			size_t buffer_size = entities.Size() * type->Size;
			byte* dest = m_storage;

			for( Entity entity : entities )
			{
				void* component_data = space.AccessComponent( entity, m_component );
				memcpy( dest, component_data, type->Size );

				dest += type->Size;
			}
		}

		TypeID Component() const
		{
			return m_component;
		}

		DataUsage Usage() const
		{
			return m_usage;
		}

		size_t Size() const
		{
			return m_count;
		}

		void* Data() const
		{
			return m_storage;
		}

	private:
		TypeID m_component { ComponentType::InvalidID };
		byte* m_storage { nullptr };

		DataUsage m_usage { DataUsage::Invalid };
		size_t m_count { 0 };
	};

	template <typename T>
	struct ReadBuffer
	{
		ReadBuffer( const ComponentDataBuffer& buffer )
			: m_buffer( buffer )
		{
			DD_ASSERT( buffer.Usage() == DataUsage::Read );
			DD_ASSERT( buffer.Component() == T::Type.ID );
		}

		ReadBuffer( const ReadBuffer& other ) :
			m_buffer( other.m_buffer )
		{
		}

		size_t Size() const
		{
			return m_buffer.Size();
		}

		const T& Get( int index ) const
		{
			DD_ASSERT( T::Type.ID == m_buffer.Component() );
			DD_ASSERT( index < m_buffer.Size() );

			return *(reinterpret_cast<T*>(m_buffer.Data()) + index);
		}

	private:
		const ComponentDataBuffer& m_buffer;
	};

	template <typename T>
	struct WriteBuffer
	{
		WriteBuffer( const ComponentDataBuffer& buffer )
			: m_buffer( buffer )
		{
			DD_ASSERT( buffer.Usage() == DataUsage::Write );
			DD_ASSERT( buffer.Component() == T::Type.ID );
		}

		WriteBuffer( const WriteBuffer& other ) : 
			m_buffer( other.m_buffer )
		{
		}

		size_t Size() const
		{
			return m_buffer.Size();
		}

		T& Get( int index ) const
		{
			DD_ASSERT( T::Type.ID == m_buffer.Component() );
			DD_ASSERT( index < m_buffer.Size() );

			return *(reinterpret_cast<T*>(m_buffer.Data()) + index);
		}

	private:
		const ComponentDataBuffer& m_buffer;
	};

	struct UpdateData
	{
		UpdateData( EntityLayer& space, dd::Span<Entity> entities, const dd::IArray<const DataRequirement*>& requirements ) :
			m_space( space ),
			m_entities( entities )
		{
			size_t entity_offset = entities.Offset();

			for( const DataRequirement* req : requirements )
			{
				const ComponentType* type = ComponentType::Types[ req->Component() ];

				byte* storage = req->GetBuffer() + (entity_offset * type->Size);

				ComponentDataBuffer data_buffer( entities, space, type->ID, req->Usage(), storage );
				m_buffers.push_back( data_buffer );
			}
		}

		template <typename T>
		ReadBuffer<T> GetRead() const
		{
			TypeID type = T::Type.ID;

			for( const ComponentDataBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == type )
				{
					DD_ASSERT( buffer.Usage() == DataUsage::Read );

					return ReadBuffer<T>( buffer );
				}
			}

			throw std::exception( "No read buffer found for component. Check your requirements!" );
		}

		template <typename T>
		WriteBuffer<T> GetWrite() const
		{
			TypeID type = T::Type.ID;

			for( const ComponentDataBuffer& buffer : m_buffers )
			{
				if( buffer.Component() == type )
				{
					DD_ASSERT( buffer.Usage() == DataUsage::Write );

					return WriteBuffer<T>( buffer );
				}
			}

			throw std::exception( "No write buffer found for component. Check your requirements!" );
		}

		void Commit()
		{
			for( const ComponentDataBuffer& buffer : m_buffers )
			{
				if( buffer.Usage() == DataUsage::Write )
				{
					CommitBuffer( buffer );
				}
			}
		}

	private:

		size_t m_entityCount { 0 };

		EntityLayer& m_space;
		dd::Span<Entity> m_entities;
		std::vector<ComponentDataBuffer> m_buffers;

		void CommitBuffer( const ComponentDataBuffer& buffer )
		{
			const ComponentType* type = ComponentType::Types[ buffer.Component() ];
			byte* src = reinterpret_cast<byte*>( buffer.Data() );

			for( Entity entity : m_entities )
			{
				void* ptr = m_space.AccessComponent( entity, type->ID );
				memcpy( ptr, src, type->Size );
			}
		}
	};

	const int PARTITION_COUNT = 4;

	void UpdateSystem( EntityLayer& space, System& system );

	void ScheduleSystemsByComponent( dd::Span<System*> systems, std::vector<System*>& out_ordered_systems );

	void ScheduleSystemsByDependencies( dd::Span<System*> systems, std::vector<System*>& out_ordered_systems );
}