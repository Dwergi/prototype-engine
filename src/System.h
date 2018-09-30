//
// System.h - Base class for systems.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#pragma once

#include "DataRequest.h"
#include "UpdateData.h"

namespace ddc
{
	struct World;

	struct System
	{
		System( const char* name ) : m_name( name ) {}

		void DependsOn( const System& system ) { DD_ASSERT( &system != this ); m_dependencies.Add( &system ); }

		const dd::IArray<const DataRequest*>& GetRequests() const { return m_requests; }
		const dd::IArray<const System*>& GetDependencies() const { return m_dependencies; }
		std::bitset<MAX_TAGS> GetRequiredTags( const char* name = nullptr ) const;
		const char* GetName() const { return m_name.c_str(); }

		virtual void Initialize( World& world ) {}
		virtual void Update( const UpdateData& data ) = 0;
		virtual void Shutdown( World& world ) {}

		int MaxPartitions() const { return m_partitions; }

	protected:

		template <typename T>
		void RequireRead( const char* name = nullptr ) 
		{ 
			CheckDuplicates( DD_TYPE( T ), DataUsage::Read, DataCardinality::Required, name );
			m_requests.Add( new ReadRequirement<T>( name ) );
		}

		template <typename T>
		void RequireWrite( const char* name = nullptr ) 
		{
			CheckDuplicates( DD_TYPE( T ), DataUsage::Write, DataCardinality::Required, name );
			m_requests.Add( new WriteRequirement<T>( name ) );
		}

		template <typename T>
		void OptionalRead( const char* name = nullptr )
		{
			CheckDuplicates( DD_TYPE( T ), DataUsage::Read, DataCardinality::Optional, name );
			m_requests.Add( new ReadOptional<T>( name ) );
		}

		template <typename T>
		void OptionalWrite( const char* name = nullptr )
		{
			CheckDuplicates( DD_TYPE( T ), DataUsage::Write, DataCardinality::Optional, name );
			m_requests.Add( new WriteOptional<T>( name ) );
		}

		bool CheckDuplicates( const dd::TypeInfo* component, DataUsage usage, DataCardinality cardinality, const char* name ) const;

		void RequireTag( Tag tag, const char* name = nullptr );

		void SetPartitions( int count )
		{
			DD_ASSERT( count > 0 && count <= MAX_PARTITIONS );
			m_partitions = count;
		}

	private:
		struct TagRequest
		{
			dd::String32 Name;
			std::bitset<MAX_TAGS> Tags;
		};

		dd::Array<const DataRequest*, MAX_COMPONENTS> m_requests;
		dd::Array<TagRequest, MAX_COMPONENTS> m_tags;

		dd::Array<const System*, 32> m_dependencies;
		dd::String32 m_name;

		const static int MAX_PARTITIONS = 8;
		int m_partitions { MAX_PARTITIONS };
	};
}