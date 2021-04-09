#include "PCH.h"
#include "Tests.h"

#include "HitTestSystem.h"
#include "JobSystem.h"

TEST_CASE( "[HitTestSystem] Execute Hit" )
{
	dd::HitTestSystem* system = new dd::HitTestSystem();

	ddm::Ray ray;
	system->ScheduleHitTest( ray );
}