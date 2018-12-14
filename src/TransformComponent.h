//
// TransformComponent.h - A component to store the transform of an object.
// Copyright (C) Sebastian Nordgren 
// April 22nd 2017
//

#pragma once

namespace dd
{
	struct TransformComponent
	{
	public:

		glm::vec3 Position;
		glm::quat Rotation;
		glm::vec3 Scale { 1.0f };

		const glm::mat4& Transform() const
		{
			//DD_ASSERT( m_transform == CalculateTransform() );
			return m_transform;
		}

		void Update()
		{
			Rotation = glm::normalize( Rotation );
			m_transform = CalculateTransform();
		}

		ALIGNED_ALLOCATORS( 16 );

		DD_CLASS( dd::TransformComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::TransformComponent, Position );
			DD_MEMBER( dd::TransformComponent, Rotation );
			DD_MEMBER( dd::TransformComponent, Scale );
		}

	private:

		glm::mat4 m_transform;

		glm::mat4 CalculateTransform() const
		{
			return glm::translate( Position ) * glm::toMat4( Rotation ) * glm::scale( Scale );
		}
	};
}