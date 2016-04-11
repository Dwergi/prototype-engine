#pragma once

#include "Component.h"
#include "DenseVectorPool.h"
#include "Vector4.h"

namespace dd
{
	class TransformComponent : public Component
	{
	public:
		glm::vec3 Position;

		typedef DenseVectorPool<TransformComponent> Pool;

		TransformComponent()
		{ 
		}
		
		BEGIN_SCRIPT_OBJECT( TransformComponent )
			PARENT( Component )
			MEMBER( TransformComponent, Position )
		END_TYPE
	};
}