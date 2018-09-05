//
// TransformComponent.h - A component to store the transform of an object.
// Copyright (C) Sebastian Nordgren 
// April 22nd 2017
//

#pragma once

#include "ComponentType.h"
#include "Vector4.h"

namespace dd
{
	struct TransformComponent
	{
	public:

		glm::mat4 Local;
		glm::mat4 World;

		void SetLocalPosition( glm::vec3 v ) { Local[ 3 ].xyz = v; }
		glm::vec3 GetLocalPosition() const { return Local[ 3 ].xyz; }

		glm::vec3 GetWorldPosition() const { return World[ 3 ].xyz; }

		DD_COMPONENT;

		ALIGNED_ALLOCATORS( 16 )
	};
}