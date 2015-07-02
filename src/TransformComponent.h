#pragma once

#include "Component.h"
#include "Vector.h"
#include "DenseVectorPool.h"

class TransformComponent : public Component
{
public:

#ifdef USE_EIGEN 
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
#endif

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