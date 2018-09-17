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
		RequireWrite<dd::BoundsComponent>();
	}

	void SceneGraphSystem::Update( const ddc::UpdateData& data )
	{
		dd::Buffer<dd::TransformComponent> transforms = data.Write<dd::TransformComponent>();
		dd::Buffer<dd::BoundsComponent> bounds = data.Write<dd::BoundsComponent>();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			const glm::mat4& t = transforms[i].Local;
			transforms[i].World = t;
			bounds[i].WorldBox = bounds[i].LocalBox.GetTransformed( t );

			bounds[i].LocalSphere = dd::BoundSphere( bounds[i].LocalBox );
			bounds[i].WorldSphere = dd::BoundSphere( bounds[i].WorldBox );
		}
	}
}
