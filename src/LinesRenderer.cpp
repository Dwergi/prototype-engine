//
// LinesComponent.h - Renderer for lines.
// Copyright (C) Sebastian Nordgren 
// November 28th 2018
//

#include "PCH.h"
#include "LinesRenderer.h"

#include "BoundBoxComponent.h"
#include "LinesComponent.h"
#include "TransformComponent.h"

namespace ddr
{
	LinesRenderer::LinesRenderer() :
		Renderer( "Lines" )
	{
		Require<dd::LinesComponent>();
		Require<dd::BoundBoxComponent>();
		Require<dd::TransformComponent>();
	}

	void LinesRenderer::RenderInit( ddc::World& world )
	{

	}

	void LinesRenderer::Render( const RenderData& render_data )
	{
		
	}
}
