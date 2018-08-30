//
// SceneGraphSystem.h - A system that maintains a scene graph.
// Copyright (C) Sebastian Nordgren 
// April 22nd 2017
//

#pragma once

#include "System.h"

namespace dd
{
	class SceneGraphSystem : public ddc::System
	{
	public:
		SceneGraphSystem();

		virtual void Update( const ddc::UpdateData& data, float dt ) override;
	};
}