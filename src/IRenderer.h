#pragma once

namespace dd
{
	class EntityManager;
	class ICamera;
	class FrameBuffer;

	class IRenderer
	{
	public:

		virtual void RenderInit( const EntityManager& entity_manager, const ICamera& camera ) {}
		virtual void Render( const EntityManager& entity_manager, const ICamera& camera ) {}
		virtual void RenderShutdown() {}

		virtual bool ShouldRenderDebug() const { return false; }
		virtual void RenderDebug() {}
	};
}