#include "PrecompiledHeader.h"

#include "catch/catch.hpp"

#include "IComponent.h"
#include "ComponentHandle.h"
#include "PackedPool.h"
#include "EntityHandle.h"
#include "EntityManager.h"

namespace
{
	struct FooComponent : dd::IComponent
	{
		using Pool = dd::PackedPool<FooComponent>;
		int ID;
	};

	struct BarComponent : dd::IComponent
	{
		using Pool = dd::PackedPool<BarComponent>;
		bool Valid;
	};
}

TEST_CASE( "[ComponentHandle] Has All Components" )
{
	REGISTER_TYPE( FooComponent );
	REGISTER_TYPE( BarComponent );

	dd::EntityManager manager;
	manager.RegisterComponent<FooComponent>();
	manager.RegisterComponent<BarComponent>();

	dd::EntityHandle entity = manager.CreateEntity();
	dd::ComponentHandle<FooComponent> foo = manager.AddComponent<FooComponent>( entity );
	
	REQUIRE( entity.Has<FooComponent>() );
	REQUIRE( entity.HasAll<FooComponent>() );

	dd::ComponentHandle<BarComponent> bar = manager.AddComponent<BarComponent>( entity );
	REQUIRE( entity.Has<BarComponent>() );
	REQUIRE( entity.HasAll<BarComponent>() );

	bool hasAll = entity.HasAll<FooComponent, BarComponent>();
	REQUIRE( hasAll );
}