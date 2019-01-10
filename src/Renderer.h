//
// Renderer.h - An interface for things that want to render something.
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#pragma once

#include "DataRequest.h"
#include "RenderData.h"

namespace ddr
{
	struct Renderer
	{
		Renderer( const char* renderer ) : m_name( renderer ) {}

		virtual void RenderInit( ddc::World& world ) {}
		virtual void RenderUpdate( ddc::World& world ) {}
		virtual void Render( const RenderData& render_data ) {}
		virtual void RenderShutdown() {}

		virtual bool ShouldRenderDebug() const { return false; }
		virtual void RenderDebug( const RenderData& render_data ) {}

		virtual bool UsesAlpha() const { return false; }

		const dd::IArray<const ddc::DataRequest*>& GetRequirements() const { return m_requests; }
		const std::bitset<ddc::MAX_TAGS>& GetRequiredTags() const { return m_tags; }

	protected:

		template <typename T>
		void Require() { m_requests.Add( new ddc::ReadRequirement<T>( nullptr ) ); }

		template <typename T>
		void Optional() { m_requests.Add( new ddc::ReadOptional<T>( nullptr ) ); }

		void RequireTag( ddc::Tag tag ) { m_tags.set( (uint) tag ); }

	private:
		dd::Array<const ddc::DataRequest*, ddc::MAX_COMPONENTS> m_requests;
		std::bitset<ddc::MAX_TAGS> m_tags;

		dd::String64 m_name;
	};
}