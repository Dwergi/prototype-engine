//
// Renderer.h - An interface for things that want to render something.
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#pragma once

#include "DataRequirement.h"
#include "RenderData.h"

namespace ddr
{
	class Renderer
	{
	public:

		virtual void RenderInit() {}
		virtual void Render( const RenderData& render_data ) {}
		virtual void RenderShutdown() {}

		virtual bool ShouldRenderDebug() const { return false; }
		virtual void RenderDebug( const RenderData& render_data ) {}

		virtual bool UsesAlpha() const { return false; }

		const dd::IArray<const ddc::DataRequirement*>& GetRequirements() const { return m_requirements; }

	protected:

		template <typename T>
		void Require() { m_requirements.Add( new ddc::ReadRequirement<T>() ); }

	private:
		dd::Array<const ddc::DataRequirement*, ddc::MAX_COMPONENTS> m_requirements;

		void CallRenderer( ddr::Renderer& renderer, const ddc::World& world, const ddr::ICamera& camera );
	};
}