#include "PCH.h"
#include "catch2/catch.hpp"

#include "JobSystem.h"
#include "ScriptSystem.h"
#include "ScriptComponent.h"
#include "Span.h"
#include "UpdateData.h"
#include "World.h"

TEST_CASE( "[ScriptSystem] Update Is Called" )
{
	dd::JobSystem jobsystem( 0 );
	ddc::World world( jobsystem );
	ddc::Entity entity = world.CreateEntity();
	dd::ScriptComponent& script = world.Add<dd::ScriptComponent>( entity );
	script.Module = "test_component";

	std::vector<ddc::Entity> entities = { entity };

	dd::Array<const ddc::DataRequest*, 1> reqs;
	reqs.Add( new ddc::WriteRequirement<dd::ScriptComponent>( nullptr ) );

	ddc::UpdateData data( world, 0 );
	data.AddData( entities, reqs, nullptr );
}