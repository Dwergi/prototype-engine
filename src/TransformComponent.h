#pragma once

#include "Component.h"
#include "DenseVectorPool.h"
#include "Vector4.h"

namespace dd
{
	class TransformComponent : public Component
	{
	public:
		Vector4 Position;

		typedef DenseVectorPool<TransformComponent> Pool;

		TransformComponent()
			: Position( 0, 0, 0, 0 )
		{ 
		}

		void Update( int iMultiplier )
		{
			Position += 0.001f;
			Position *= (float) iMultiplier;
		}

		BEGIN_MEMBERS( TransformComponent )
			MEMBER( TransformComponent, Vector4, Position, "Position" );
		END_MEMBERS
	};
}