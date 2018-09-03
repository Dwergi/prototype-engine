#pragma once

#include "DataRequirement.h"
#include "UpdateData.h"

namespace ddc
{
	struct World;

	struct System
	{
		System( const char* name ) : m_name( name ) {}

		void DependsOn( const System& system ) { DD_ASSERT( &system != this ); m_dependencies.Add( &system ); }

		const dd::IArray<const DataRequirement*>& GetRequirements() const { return m_requirements; }
		const dd::IArray<const System*>& GetDependencies() const { return m_dependencies; }

		virtual void Initialize( World& world ) {}
		virtual void Update( const UpdateData& data, float delta_t ) = 0;
		virtual void Shutdown( World& world ) {}

		int MaxPartitions() const { return m_partitions; }

	protected:

		template <typename T>
		void RequireRead() { m_requirements.Add( new ReadRequirement<T>() ); }

		template <typename T>
		void RequireWrite() { m_requirements.Add( new WriteRequirement<T>() ); }

		void SetPartitions( int count )
		{
			DD_ASSERT( count > 0 && count <= MAX_PARTITIONS );
			m_partitions = count;
		}

	private:
		dd::Array<const DataRequirement*, MAX_COMPONENTS> m_requirements;
		dd::Array<const System*, 32> m_dependencies;
		dd::String64 m_name;

		const static int MAX_PARTITIONS = 8;
		int m_partitions { MAX_PARTITIONS };
	};
}