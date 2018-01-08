#pragma once

namespace dd
{
	class EntityManager;
	class ICamera;

	class IRenderer
	{
	public:

		virtual void RenderInit( const EntityManager& entity_manager, const ICamera& camera ) {}
		virtual void Render( const EntityManager& entity_manager, const ICamera& camera ) {}
		virtual void RenderShutdown() {}
	};
}