//
// TransformComponent.h - A component to store the transform of an object.
// Copyright (C) Sebastian Nordgren 
// April 22nd 2017
//

#pragma once

#include "ComponentBase.h"
#include "DenseVectorPool.h"
#include "Vector4.h"

namespace dd
{
	class TransformComponent : public ComponentBase
	{
	public:

		typedef DenseVectorPool<TransformComponent> Pool;

		TransformComponent();
		TransformComponent( const TransformComponent& other );

		void SetLocalPosition( const glm::vec3& pos );
		void SetLocalTransform( const glm::mat4& transform );

		glm::vec3 GetLocalPosition() const { return m_local[3].xyz(); }
		const glm::mat4& GetLocalTransform() const { return m_local; }

		glm::vec3 GetWorldPosition() const { return m_world[3].xyz(); }
		const glm::mat4& GetWorldTransform() const { return m_world; }

		void UpdateWorldTransform();

		ALIGNED_ALLOCATORS( 16 )
		
		BEGIN_SCRIPT_OBJECT( TransformComponent )
			PARENT( ComponentBase )
		END_TYPE

	private:

		EntityHandle m_parent;
		bool m_dirty;
		glm::mat4 m_local;
		glm::mat4 m_world;
	};
}