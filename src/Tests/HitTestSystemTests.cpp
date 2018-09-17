#include "PrecompiledHeader.h"
#include "catch2/catch.hpp"

#include "HitTestSystem.h"
#include "JobSystem.h"
#include "World.h"

TEST_CASE( "[HitTestSystem] Execute Hit" )
{
	dd::HitTestSystem* system = new dd::HitTestSystem();

	dd::Ray ray;
	system->ScheduleHitTest( ray, 10 );
}