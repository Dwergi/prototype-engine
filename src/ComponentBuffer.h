//
// ComponentBuffer.h
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#pragma once

#include "DataRequest.h"

namespace ddc
{
	struct ComponentBuffer
	{
		ComponentBuffer();
		explicit ComponentBuffer(DataRequest* request);
		ComponentBuffer(ComponentBuffer&& other) noexcept;

		const dd::TypeInfo* Component() const { return m_request->Component(); }
		DataUsage Usage() const { return m_request->Usage(); }

		size_t Size() const { return m_count; }
		byte* Data() const { return m_storage; }

		bool Optional() const { return m_request->Optional(); }
		bool Has( size_t i ) const { return !Optional() || m_exists.at( i ); }
		
	private:

		friend struct UpdateBuffer;

		void Fill(const std::vector<Entity>& entities);
		void Commit(const std::vector<ddc::Entity>& entities);
		
		DataRequest* m_request { nullptr };
		size_t m_count { 0 };

		std::vector<bool> m_exists;
		byte* m_storage { nullptr };
	};
}