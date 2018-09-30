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

		glm::mat4 Transform;

		void SetPosition( glm::vec3 v ) { Transform[ 3 ].xyz = v; }
		glm::vec3 GetPosition() const { return Transform[ 3 ].xyz; }

		ALIGNED_ALLOCATORS( 16 );

		DD_CLASS( dd::TransformComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::TransformComponent, Transform );
		}
	};
}