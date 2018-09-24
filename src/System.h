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
		const std::bitset<MAX_TAGS>& GetRequiredTags() const { return m_tags; }
		const char* GetName() const { return m_name.c_str(); }

		virtual void Initialize( World& world ) {}
		virtual void Update( const UpdateData& data ) = 0;
		virtual void Shutdown( World& world ) {}

		int MaxPartitions() const { return m_partitions; }

	protected:

		template <typename T>
		void RequireRead() { m_requests.Add( new ReadRequirement<T>() ); }

		template <typename T>
		void RequireWrite() { m_requests.Add( new WriteRequirement<T>() ); }

		template <typename T>
		void OptionalRead() { m_requests.Add( new ReadOptional<T>() ); }

		template <typename T>
		void OptionalWrite() { m_requests.Add( new WriteOptional<T>() ); }

		void RequireTag( Tag tag ) { m_tags.set( (uint) tag ); }

		void SetPartitions( int count )
		{
			DD_ASSERT( count > 0 && count <= MAX_PARTITIONS );
			m_partitions = count;
		}

	private:
		dd::Array<const DataRequest*, MAX_COMPONENTS> m_requests;
		std::bitset<MAX_TAGS> m_tags;

		dd::Array<const System*, 32> m_dependencies;
		dd::String64 m_name;

		const static int MAX_PARTITIONS = 8;
		int m_partitions { MAX_PARTITIONS };
	};
}