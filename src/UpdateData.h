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
		UpdateData( ddc::EntitySpace& entities, const dd::Input& input, float delta_t );
		UpdateData( const UpdateData& other ) = delete;

		float Delta() const { return m_delta; }
		ddc::EntitySpace& EntitySpace() const { return m_world; }
		const UpdateDataBuffer& Data( const char* name = nullptr ) const;
		const std::vector<dd::InputAction>& InputActions() const { return m_input.GetInputActions(); }


	private:

		friend struct System; // for AddData and Commit

		void AddData(const std::vector<Entity>& entities, const dd::IArray<const DataRequest*>& requests, const char* name);
		void Commit();

		float m_delta { 0 };
		ddc::EntitySpace& m_world;
		dd::Input& m_input;
		std::vector<UpdateDataBuffer> m_dataBuffers;
	};
}