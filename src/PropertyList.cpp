//
// PropertyList.cpp - A list of properties that can be instantiated from a type and bound to an instance.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//
#include "PCH.h"
#include "PropertyList.h"

namespace
{
	void* AddPointer(void* object, uint64 offset)
	{
		return (void*) (((byte*) object) + offset);
	}
}

namespace dd
{
	PropertyList::PropertyList(PropertyList&& other) noexcept
	{
		m_properties = std::move(other.m_properties);
		m_type = other.m_type;
		m_object = other.m_object;
	}

	PropertyList::~PropertyList()
	{
		m_properties.clear();
		m_type = nullptr;
		m_object = nullptr;
	}

	std::optional<Property> PropertyList::Find(std::string_view name) const
	{
		for (const Property& prop : m_properties)
		{
			if (prop.Name() == name)
			{
				return std::optional<Property>(prop);
			}
		}

		return std::optional<Property>();
	}

	//
	// Recursively add members to the property list.
	//
	void PropertyList::AddMembers(const TypeInfo* typeInfo, void* object, const String& prefix)
	{
		if (typeInfo == nullptr)
			return;

		for (Member& member : typeInfo->Members())
		{
			String128 member_name;
			if (prefix.Length() > 0)
			{
				member_name = prefix;
				member_name += ".";
				member_name += member.Name();
			}
			else
			{
				member_name = member.Name();
			}

			m_properties.emplace_back(Variable(member, object), member_name.c_str());

			void* new_object = PointerAdd(object, member.Offset());
			AddMembers(member.Type(), new_object, member_name);
		}
	}
}