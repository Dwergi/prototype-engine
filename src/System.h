#pragma once

namespace ddc
{
	struct DataRequirement;
	struct UpdateData;

	struct System
	{
		System( const char* name ) : m_name( name ) {}

		void RegisterDependency( const System& system ) { DD_ASSERT( &system != this ); m_dependencies.Add( &system ); }

		void RegisterDataRequirement( const DataRequirement& req ) { m_requirements.Add( &req ); }

		const dd::IArray<const DataRequirement*>& GetRequirements() const { return m_requirements; }
		const dd::IArray<const System*>& GetDependencies() const { return m_dependencies; }

		virtual void Initialize() {}
		virtual void Update( const UpdateData& data ) = 0;
		virtual void Shutdown() {}

	private:
		dd::Array<const DataRequirement*, MAX_COMPONENTS> m_requirements;
		dd::String64 m_name;
		dd::Array<const System*, 32> m_dependencies;
	};
}