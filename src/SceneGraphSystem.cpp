//
// SceneGraphSystem.cpp - A system that maintains a scene graph.
// Copyright (C) Sebastian Nordgren 
// April 22nd 2017
//

#include "PrecompiledHeader.h"
#include "SceneGraphSystem.h"

#include "BoundsComponent.h"
#include "TransformComponent.h"

namespace dd
{
	SceneGraphSystem::SceneGraphSystem() : 
		ddc::System( "Scene Graph" )
	{
		RequireWrite<dd::TransformComponent>();
		OptionalWrite<dd::BoundsComponent>();
	}

	void SceneGraphSystem::Update( const ddc::UpdateData& data )
	{
		auto transforms = data.Write<dd::TransformComponent>();
		auto bounds = data.Write<dd::BoundsComponent>();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			const glm::mat4& t = transforms[i].Local;
			transforms[i].World = t;

			dd::BoundsComponent* bounds_cmp = bounds.Access( i );
			if( bounds_cmp != nullptr )
			{
				bounds_cmp->WorldBox = bounds_cmp->LocalBox.GetTransformed( t );

				bounds_cmp->LocalSphere = dd::BoundSphere( bounds_cmp->LocalBox );
				bounds_cmp->WorldSphere = dd::BoundSphere( bounds_cmp->WorldBox );
			}
		}
	}
}
