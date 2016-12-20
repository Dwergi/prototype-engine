#pragma once

#include "ComponentBase.h"
#include "DenseVectorPool.h"
#include "Vector4.h"

namespace dd
{
	class TransformComponent : public ComponentBase
	{
	public:
		glm::mat4 Transform;

		typedef DenseVectorPool<TransformComponent> Pool;

		TransformComponent() { }
		TransformComponent( const TransformComponent& other ) : ComponentBase( other ), Transform( other.Transform ) {}

		glm::vec3 GetPosition() const { return Transform[3].xyz(); }
		void SetPosition( const glm::vec3& pos ) { Transform[3].xyz = pos; }

		ALIGNED_ALLOCATORS( 16 )
		
		BEGIN_SCRIPT_OBJECT( TransformComponent )
			PARENT( ComponentBase )
		END_TYPE
	};
}