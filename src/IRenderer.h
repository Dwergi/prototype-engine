//
// IRenderer.h - An interface for things that want to render something.
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#pragma once

namespace ddr
{
	class ICamera;
	class FrameBuffer;
	struct RenderData;
	struct UniformStorage;

	class IRenderer
	{
	public:

		virtual void RenderInit() {}
		virtual void Render( const RenderData& render_data ) {}
		virtual void RenderShutdown() {}

		virtual bool ShouldRenderDebug() const { return false; }
		virtual void RenderDebug() {}

		virtual bool UsesAlpha() const { return false; }
	};
}