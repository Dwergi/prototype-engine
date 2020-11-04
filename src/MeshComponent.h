//
// MeshComponent.h - A component that specifies that a certain mesh should be drawn at the given location.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#pragma once

#include "ddr/Mesh.h"

namespace dd
{
	struct MeshComponent
	{
	public: 

		ddr::MeshHandle Mesh;

		DD_BEGIN_CLASS( dd::MeshComponent )
			DD_COMPONENT();

			DD_MEMBER( Mesh );
		DD_END_CLASS()
	};
}