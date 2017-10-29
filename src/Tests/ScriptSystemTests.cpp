#include "PrecompiledHeader.h"
#include "catch/catch.hpp"

#include "EntityManager.h"
#include "ScriptSystem.h"
#include "ScriptComponent.h"

TEST_CASE( "[ScriptSystem] Update Is Called" )
{
	dd::EntityManager entityManager;
	
	entityManager.RegisterComponent<dd::ScriptComponent>();

	dd::EntityHandle handle = entityManager.Create();
	dd::ComponentHandle<dd::ScriptComponent> script = entityManager.AddComponent<dd::ScriptComponent>( handle );
	script.Write()->SetModule( dd::String32( "test_component" ) );


}