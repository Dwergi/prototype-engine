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

		virtual FrameBuffer* GetFrameBuffer() { return nullptr; }
		virtual bool ShouldRenderFrameBuffer() const { return false; }
	};
}