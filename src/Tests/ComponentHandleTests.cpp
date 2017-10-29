#include "PrecompiledHeader.h"

#include "catch/catch.hpp"

#include "ComponentBase.h"
#include "ComponentHandle.h"
#include "DenseVectorPool.h"
#include "EntityHandle.h"
#include "EntityManager.h"

namespace
{
	struct FooComponent : dd::ComponentBase
	{
		using Pool = dd::DenseVectorPool<FooComponent>;
		int ID;
	};

	struct BarComponent : dd::ComponentBase
	{
		using Pool = dd::DenseVectorPool<BarComponent>;
		bool Valid;
	};
}

TEST_CASE( "[ComponentHandle] Has All Components" )
{
	REGISTER_TYPE( FooComponent );
	REGISTER_TYPE( BarComponent );

	FooComponent::Pool fooPool;
	BarComponent::Pool barPool;

	dd::EntityManager manager;
	dd::EntityHandle entity = manager.CreateEntity();

	FooComponent* foo = fooPool.Create( entity );

	REQUIRE( entity.Has<FooComponent>() );
	REQUIRE( entity.HasAll<FooComponent>() );

	BarComponent* bar = barPool.Create( entity );
	
	REQUIRE( entity.Has<BarComponent>() );

	bool hasAll = entity.HasAll<FooComponent, BarComponent>();
	REQUIRE( hasAll );
}