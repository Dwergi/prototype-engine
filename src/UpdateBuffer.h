#pragma once

#include "ComponentBuffer.h"
#include "ComponentViews.h"

namespace ddr
{
	struct RenderData;
	struct IRenderer;
}

namespace ddc
{
	struct DataRequest;
	struct System;

	//
	// Update buffers are named sets of entities a System requests that have certain sets of components and tags.
	// Eg. "static_meshes" might contain MeshComponent, TransformComponent and the Static tag. 
	// These store ALL entities that match the criteria. They should typically not be accessed by Systems directly.
	//
	struct UpdateBuffer
	{
		UpdateBuffer() {}
		explicit UpdateBuffer(const char* name);
		UpdateBuffer(UpdateBuffer&& other);

		const dd::String& Name() const { return m_name; }

		size_t Size() const { return m_entities.size(); }
		const std::vector<Entity>& Entities() const { return m_entities; }
		std::vector<Entity>& Entities() { return m_entities; }

		template <typename T>
		const ddc::ComponentBuffer& GetBuffer(DataUsage usage) const
		{
			const dd::TypeInfo* type = DD_FIND_TYPE(T);

			for (const ComponentBuffer& buffer : m_buffers)
			{
				if (buffer.Component() == type &&
					buffer.Usage() == usage)
				{
					return buffer;
				}
			}
			throw std::exception("No buffer found for given component. Check your requests!");
		}

	private:
		static const int MAX_BUFFERS = 16;

		friend System;
		friend ddr::RenderData;
		friend ddr::IRenderer;

		dd::String32 m_name;
		std::vector<Entity> m_entities;

		dd::Array<ComponentBuffer, MAX_BUFFERS> m_buffers;
		dd::Array<DataRequest*, MAX_BUFFERS> m_requests;
		dd::Array<dd::ComponentID, MAX_BUFFERS> m_requiredComponents;
		TagBits m_tags;

		void RequestData(DataRequest* request);
		void RequireTag(ddc::Tag tag) { m_tags.set((uint) tag); }
		void Fill(ddc::EntityLayer& layer);
		bool CheckDuplicates(const dd::TypeInfo* component, ddc::DataUsage usage, ddc::DataCardinality cardinality) const;
		void Commit();
	};

	//
	// Update buffer views provide a lightweight view over a range of the data in an update buffer.
	// This allows for updates to be split across multiple threads, 
	// since each one should only touch a contiguous, known range of entities and components.
	// 
	struct UpdateBufferView
	{
		UpdateBufferView() {}
		UpdateBufferView(UpdateBuffer& buffer, size_t start, size_t count);
		UpdateBufferView(const UpdateBufferView& other);

		const dd::Span<Entity>& Entities() const { return m_entities; }
		size_t Size() const { return m_entities.Size(); }
		const dd::String& Name() const { return m_buffer->Name(); }

		template <typename T>
		ddc::ReadView<T> Read() const
		{
			const ddc::ComponentBuffer& cmp_buffer = m_buffer->GetBuffer<T>(DataUsage::Read);
			return ddc::ReadView<T>(cmp_buffer, m_start, m_count);
		}

		template <typename T>
		ddc::WriteView<T> Write() const
		{
			const ddc::ComponentBuffer& cmp_buffer = m_buffer->GetBuffer<T>(DataUsage::Write);
			return ddc::WriteView<T>(cmp_buffer, m_start, m_count);
		}

	private:

		UpdateBuffer* m_buffer { nullptr };
		size_t m_start { 0 };
		size_t m_count { 0 };
		dd::Span<ddc::Entity> m_entities;
	};
}