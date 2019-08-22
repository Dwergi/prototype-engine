//
// RenderData.h - Data used for renderer updates.
// Copyright (C) Sebastian Nordgren 
// August 26th 2018
//

#pragma once

#include "ICamera.h"
#include "Uniforms.h"
#include "UpdateDataBuffer.h"

namespace ddc
{
	struct DataRequest;
	struct Entity;
	struct EntityLayer;
}

namespace ddr
{
	struct RenderData : public ddc::UpdateDataBuffer
	{
	public:
		RenderData();
		RenderData(RenderData&& data);

		const ddc::EntityLayer& EntityLayer() const { return *m_layer; }
		ddc::EntityLayer& EntityLayer() { return *m_layer; }

		const ddr::ICamera& Camera() const { return *m_camera; }
		ddr::UniformStorage& Uniforms() const { return *m_uniforms; }
		float Delta() const { return m_delta; }

		template <typename T>
		ddc::ReadView<T> Get() const { return Read<T>(); }

	private:
		friend struct RenderManager;
		friend struct IRenderer;

		ddc::EntityLayer* m_layer { nullptr };
		const ddr::ICamera* m_camera { nullptr };
		ddr::UniformStorage* m_uniforms { nullptr };
		float m_delta { 0 };

		void Fill(ddc::EntityLayer& layer, const ddr::ICamera& camera, ddr::UniformStorage& uniforms, float delta_t);
	};
}