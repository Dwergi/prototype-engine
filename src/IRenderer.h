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
	struct IRenderer
	{
		IRenderer( const char* renderer ) : m_name( renderer ) {}

		virtual void Initialize() {} 
		virtual void Shutdown() {}

		virtual void Update(RenderData& render_data) {}
		virtual void Render(const RenderData& render_data) {}

		virtual bool ShouldRenderDebug() const { return false; }
		virtual void RenderDebug( const RenderData& render_data ) {}

		virtual bool UsesAlpha() const { return false; }

		const dd::IArray<ddc::DataRequest*>& GetRequirements() const { return m_requests; }
		const std::bitset<ddc::MAX_TAGS>& GetRequiredTags() const { return m_tags; }

	protected:

		template <typename T>
		void Require()
		{
			const dd::TypeInfo* type = dd::ComponentRegistration<T>::Register();
			CheckDuplicates(type, ddc::DataUsage::Read, ddc::DataCardinality::Required);
			m_requests.Add( new ddc::ReadRequired<T>( nullptr ) );
		}

		template <typename T>
		void Optional()
		{
			const dd::TypeInfo* type = dd::ComponentRegistration<T>::Register();
			CheckDuplicates(type, ddc::DataUsage::Read, ddc::DataCardinality::Optional);
			m_requests.Add(new ddc::ReadOptional<T>(nullptr));
		}

		void RequireTag( ddc::Tag tag ) { m_tags.set( (uint) tag ); }

	private:
		friend struct RenderManager;

		ddr::RenderData m_data;
		dd::Array<ddc::DataRequest*, ddc::MAX_COMPONENTS> m_requests;
		std::bitset<ddc::MAX_TAGS> m_tags;

		dd::String64 m_name;

		ddr::RenderData& AccessRenderData() { return m_data; }

		bool CheckDuplicates(const dd::TypeInfo* component, ddc::DataUsage usage, ddc::DataCardinality cardinality);
	};
}