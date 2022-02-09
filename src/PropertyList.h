//
// PropertyList.h - A list of properties that can be instantiated from a type and bound to an instance.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#pragma once

#include "Property.h"
#include "TypeInfo.h"
#include "Vector.h"
#include <optional>

namespace dd
{
	class PropertyList
	{
	public:
		template <typename T>
		explicit PropertyList(T& host) :
			m_type(DD_FIND_TYPE(T)),
			m_object(&host)
		{
			AddMembers(m_type, m_object);
		}

		PropertyList(PropertyList&& other) noexcept;
		~PropertyList();

		std::optional<Property> Find(std::string_view name) const;

		void* Instance() const { return m_object; }
		const TypeInfo* Type() const { return m_type; }
		uint64 Size() const { return m_properties.size(); }

	protected:
		std::vector<Property> m_properties;
		const TypeInfo* m_type { nullptr };
		void* m_object { nullptr };

		void AddMembers(const TypeInfo* typeInfo, void* base, const String& prefix = dd::String8());
	};
}