//
// UpdateData.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "Input.h"
#include "UpdateDataBuffer.h"

namespace ddc
{
	struct UpdateData
	{
		UpdateData( ddc::EntityLayer& layer, float delta_t );
		UpdateData( const UpdateData& other ) = delete;

		float Delta() const { return m_delta; }
		ddc::EntityLayer& EntityLayer() const { return m_layer; }
		const UpdateDataBuffer& Data( const char* name = nullptr ) const;

	private:

		friend struct SystemsManager; // for AddData and Commit

		void AddData(const std::vector<Entity>& entities, const dd::IArray<DataRequest*>& requests, const char* name);
		void Commit();

		float m_delta { 0 };
		ddc::EntityLayer& m_layer;
		std::vector<UpdateDataBuffer> m_dataBuffers;
	};
}