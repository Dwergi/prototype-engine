#pragma once

#include "ComponentBuffer.h"

namespace ddr
{
	struct RenderData;
	struct IRenderer;
}

namespace ddc
{
	struct DataRequest;

	struct UpdateDataBuffer
	{
		UpdateDataBuffer() {}
		explicit UpdateDataBuffer(const char* name);
		UpdateDataBuffer(UpdateDataBuffer&& other);

		const std::vector<Entity>& Entities() const { return m_entities; }
		size_t Size() const { return m_entities.size(); }
		const dd::String& Name() const { return m_name; }

		template <typename T>
		ddc::ReadView<T> Read() const
		{
			const dd::TypeInfo* type = DD_FIND_TYPE(T);

			for (const ComponentBuffer& buffer : m_buffers)
			{
				if (buffer.Component() == *type &&
					buffer.Usage() == DataUsage::Read)
				{
					return ddc::ReadView<T>(buffer);
				}
			}
			throw std::exception("No read buffer found for component. Check your requests!");
		}

		template <typename T>
		ddc::WriteView<T> Write() const
		{
			const dd::TypeInfo* type = DD_FIND_TYPE(T);

			for (const ComponentBuffer& buffer : m_buffers)
			{
				if (buffer.Component() == *type &&
					buffer.Usage() == DataUsage::Write)
				{
					return ddc::WriteView<T>(buffer);
				}
			}
			throw std::exception("No write buffer found for component. Check your requests!");
		}

	private:

		static const int MAX_BUFFERS = 16;

		// TODO: This seems like an excessive amount of friends...
		friend struct UpdateData;
		friend struct System;
		friend ddr::RenderData;
		friend ddr::IRenderer;

		dd::String16 m_name;
		std::vector<Entity> m_entities;

		dd::Array<ComponentBuffer, MAX_BUFFERS> m_buffers;
		dd::Array<DataRequest*, MAX_BUFFERS> m_requests;
		dd::Array<dd::ComponentID, MAX_BUFFERS> m_requiredComponents;
		TagBits m_tags;

		void RequestData(DataRequest* request);
		void RequireTag(ddc::Tag tag) { m_tags.set(( uint) tag); }
		void Fill(ddc::EntityLayer& layer);
		bool CheckDuplicates(const dd::TypeInfo& component, ddc::DataUsage usage, ddc::DataCardinality cardinality) const;
		void Commit();
	};
}