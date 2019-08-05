#include "PCH.h"
#include "catch2/catch.hpp"

#include "JobSystem.h"
#include "ScriptSystem.h"
#include "ScriptComponent.h"
#include "Span.h"
#include "UpdateData.h"

TEST_CASE( "[ScriptSystem] Update Is Called" )
{
	dd::JobSystem jobsystem( 0 );
	ddc::EntitySpace entities( jobsystem );
	ddc::Entity entity = entities.CreateEntity();
	dd::ScriptComponent& script = entities.Add<dd::ScriptComponent>( entity );
	script.Module = "test_component";

	std::vector<ddc::Entity> entities = { entity };

	dd::Array<const ddc::DataRequest*, 1> reqs;
	reqs.Add( new ddc::WriteRequirement<dd::ScriptComponent>( nullptr ) );

	ddc::UpdateData data( entities, 0 );
	data.AddData( entities, reqs, nullptr );
}