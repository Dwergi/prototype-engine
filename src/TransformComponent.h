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

		void SetPosition( float x, float y )
		{
			Position.X = x;
			Position.Y = y;
		}

		BEGIN_SCRIPT_OBJECT( TransformComponent )
			PARENT( Component );
			MEMBER( Position );
			METHOD( SetPosition );
		END_TYPE
	};
}