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
	struct EntitySpace;

	struct System
	{
		System(const char* name);

		virtual void Initialize(EntitySpace& space) {}
		virtual void Update(const UpdateData& update_data) = 0;
		virtual void Shutdown(EntitySpace& space) {}

		//
		// Allow the system to be updated for the given space. May be called multiple times. 
		// If never called, then system will allow all spaces.
		//
		System& EnableForSpace(EntitySpace& space);

		void DependsOn( const System& system ) { DD_ASSERT( &system != this ); m_dependencies.Add( &system ); }

		const dd::Vector<const DataRequest*>& GetRequests() const { return m_requests; }
		const dd::Vector<const System*>& GetDependencies() const { return m_dependencies; }
		std::bitset<MAX_TAGS> GetRequiredTags( const char* name = nullptr ) const;
		const char* GetName() const { return m_name.c_str(); }

		int MaxPartitions() const { return m_partitions; }

	protected:

		template <typename T>
		void RequireRead( const char* name = nullptr ) 
		{ 
			CheckDuplicates( DD_FIND_TYPE( T ), DataUsage::Read, DataCardinality::Required, name );
			m_requests.Add( new ReadRequirement<T>( name ) );
		}

		template <typename T>
		void RequireWrite( const char* name = nullptr ) 
		{
			CheckDuplicates( DD_FIND_TYPE( T ), DataUsage::Write, DataCardinality::Required, name );
			m_requests.Add( new WriteRequirement<T>( name ) );
		}

		template <typename T>
		void OptionalRead( const char* name = nullptr )
		{
			CheckDuplicates( DD_FIND_TYPE( T ), DataUsage::Read, DataCardinality::Optional, name );
			m_requests.Add( new ReadOptional<T>( name ) );
		}

		template <typename T>
		void OptionalWrite( const char* name = nullptr )
		{
			CheckDuplicates( DD_FIND_TYPE( T ), DataUsage::Write, DataCardinality::Optional, name );
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

		dd::Vector<const DataRequest*> m_requests;
		dd::Vector<TagRequest> m_tags;

		dd::Vector<const System*> m_dependencies;
		dd::String32 m_name;

		const static int MAX_PARTITIONS = 8;
		int m_partitions { MAX_PARTITIONS };
	};
}