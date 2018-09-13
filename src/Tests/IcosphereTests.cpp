#include "PrecompiledHeader.h"
#include "catch2/catch.hpp"

#include "Icosphere.h"
#include "Mesh.h"

TEST_CASE( "[Icosphere] 1 Iteration" )
{
	ddr::Mesh* mesh = nullptr;
	dd::MakeIcosphere( *mesh, 1 );
}