//
// Renderer.h - An interface for things that want to render something.
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#pragma once

#include "ddc/DataRequest.h"
#include "ddr/RenderData.h"

namespace ddr
{
	struct FrameBuffer;

	struct IRenderer
	{
		IRenderer(const char* name) : m_name(name) {}

		virtual void Initialize() {} 
		virtual void Shutdown() {}

		virtual void Update(RenderData& render_data) {}
		virtual void Render(const RenderData& render_data) {}

		virtual bool ShouldRenderDebug() const { return false; }
		virtual void RenderDebug(const RenderData& render_data, FrameBuffer& dest) {}

		virtual bool UsesAlpha() const { return false; }

	protected:

		template <typename T>
		void Require()
		{
			const dd::TypeInfo* type = dd::ComponentRegistration<T>::Register();
			m_data.RequestData(new ddc::ReadRequired<T>());
		}

		template <typename T>
		void Optional()
		{
			const dd::TypeInfo* type = dd::ComponentRegistration<T>::Register();
			m_data.RequestData(new ddc::ReadOptional<T>());
		}

		void RequireTag(ddc::Tag tag) { m_data.RequireTag(tag); }

	private:
		friend struct RenderManager;

		dd::String64 m_name;
		ddr::RenderData m_data;

		const ddr::RenderData& RenderData() const { return m_data; }
		ddr::RenderData& RenderData() { return m_data; }
	};
}