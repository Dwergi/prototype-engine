//
// IRenderer.h - An interface for things that want to render something.
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#pragma once

namespace ddr
{
	class UniformStorage;
}

namespace dd
{
	class ICamera;
	class EntityManager;
	class FrameBuffer;

	class IRenderer
	{
	public:

		virtual void RenderInit() {}
		virtual void Render( const EntityManager& entity_manager, const ICamera& camera, ddr::UniformStorage& uniforms ) {}
		virtual void RenderShutdown() {}

		virtual bool ShouldRenderDebug() const { return false; }
		virtual void RenderDebug() {}
	};
}