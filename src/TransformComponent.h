#pragma once

#include "Component.h"
#include "DenseVectorPool.h"
#include "Vector4.h"

namespace dd
{
	class TransformComponent : public Component
	{
	public:
		glm::mat4 Transform;

		typedef DenseVectorPool<TransformComponent> Pool;

		TransformComponent() { }
		TransformComponent( const TransformComponent& other ) : Component( other ), Transform( other.Transform ) {}

		glm::vec3 GetPosition() const { return Transform[3].xyz(); }
		void SetPosition( const glm::vec3& pos ) { Transform[3].xyz = pos; }
		
		BEGIN_SCRIPT_OBJECT( TransformComponent )
			PARENT( Component )
		END_TYPE
	};
}