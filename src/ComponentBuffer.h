#pragma once

#include "DataRequirement.h"
#include "World.h"

namespace ddc
{
	struct ComponentBuffer
	{
		ComponentBuffer( dd::Span<Entity> entities, const World& world, const ComponentType& component, DataUsage usage, byte* storage );

		const ComponentType& Component() const { return m_component; }
		DataUsage Usage() const { return m_usage; }
		size_t Size() const { return m_count; }
		byte* Data() const { return m_storage; }
		
	private:
		const ComponentType& m_component;
		byte* m_storage { nullptr };

		DataUsage m_usage { DataUsage::Invalid };
		size_t m_count { 0 };
	};
}