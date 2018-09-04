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

		Renderer( const char* renderer ) : m_name( renderer ) {}

		virtual void RenderInit() {}
		virtual void Render( const RenderData& render_data ) {}
		virtual void RenderShutdown() {}

		virtual bool ShouldRenderDebug() const { return false; }
		virtual void RenderDebug( const RenderData& render_data ) {}

		virtual bool UsesAlpha() const { return false; }

		const dd::IArray<const ddc::DataRequirement*>& GetRequirements() const { return m_requirements; }
		const std::bitset<ddc::MAX_TAGS>& GetRequiredTags() const { return m_tags; }

	protected:

		template <typename T>
		void Require() { m_requirements.Add( new ddc::ReadRequirement<T>() ); }

		void RequireTag( ddc::Tag tag ) { m_tags.set( (uint) tag ); }

	private:
		dd::Array<const ddc::DataRequirement*, ddc::MAX_COMPONENTS> m_requirements;
		std::bitset<ddc::MAX_TAGS> m_tags;

		dd::String64 m_name;
	};
}