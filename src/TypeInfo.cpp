//
// TypeInfo.cpp - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
// Significantly influenced by Randy Gaul (http://RandyGaul.net)
//

#include "PCH.h"
#include "TypeInfo.h"

#include "nlohmann/json.hpp"

namespace dd
{
	std::unordered_map<String64, TypeInfo*>* TypeInfo::sm_typeMap = nullptr;
	std::vector<std::function<void()>>* TypeInfo::sm_registrations = nullptr;
	std::vector<TypeInfo*>* TypeInfo::sm_components = nullptr;
	bool TypeInfo::sm_defaultsRegistered = false;

	uint TypeInfo::sm_maxID = 0;
	uint8 TypeInfo::sm_maxComponentID = 0;

	void FindNamespace(const char* name, String& nameSpace, String& typeName)
	{
		String128 input(name);
		uint64 offset = 0;
		while (true)
		{
			uint64 found = input.Find("::", offset);
			if (found == String::Invalid)
			{
				break;
			}

			offset = found + 2;
		}

		if (offset > 0)
		{
			nameSpace = input.Substring(0, offset - 2);
			typeName = input.Substring(offset);
		}
		else
		{
			nameSpace = "";
			typeName = input;
		}
	}

	TypeInfo::TypeInfo()
	{
		if (sm_typeMap == nullptr)
		{
			sm_typeMap = new std::unordered_map<String64, TypeInfo*>();
		}

		if (sm_registrations == nullptr)
		{
			sm_registrations = new std::vector<std::function<void()>>();
		}

		if (sm_components == nullptr)
		{
			sm_components = new std::vector<TypeInfo*>();
		}
	}

	void TypeInfo::Initialize(const char* name, uint size)
	{
		String128 typeName, nameSpace;
		FindNamespace(name, nameSpace, typeName);

		m_name = typeName;
		m_namespace = nameSpace;
		m_size = size;

		m_id = sm_maxID;
		++sm_maxID;
	}

	const Member* TypeInfo::GetMember(const char* memberName) const
	{
		for (const Member& member : m_members)
		{
			if (member.m_name == memberName)
				return &member;
		}

		return nullptr;
	}

	const Function* TypeInfo::GetMethod(const char* methodName) const
	{
		for (const Method& m : m_methods)
		{
			if (m.Name == methodName)
				return &m.Function;
		}

		return nullptr;
	}

	const TypeInfo* TypeInfo::GetType(const char* typeName)
	{
		return GetType(String64(typeName));
	}

	const TypeInfo* TypeInfo::GetType(const String& typeName)
	{
		auto it = sm_typeMap->find(typeName);
		if (it == sm_typeMap->end())
		{
			DD_ASSERT(false);
			return nullptr;
		}

		return it->second;
	}

	bool TypeInfo::IsDerivedFrom(const TypeInfo* type) const
	{
		const TypeInfo* current = this;

		while (current != nullptr)
		{
			if (current == type)
				return true;

			current = current->m_parentType;
		}

		return false;
	}

	void TypeInfo::RegisterComponent()
	{
		if (m_componentID == INVALID_COMPONENT)
		{
			m_componentID = sm_maxComponentID;
			sm_components->push_back(this);

			++sm_maxComponentID;
		}
	}

	const TypeInfo* TypeInfo::GetComponent(dd::ComponentID id)
	{
		const TypeInfo* type = sm_components->at(id);
		return type;
	}

	String128 TypeInfo::FullTypeName() const
	{
		String128 fullName;
		fullName += m_namespace.c_str();
		fullName += "::";
		fullName += m_name.c_str();

		return fullName;
	}

	bool TypeInfo::operator==(const TypeInfo& other) const
	{
		return this == &other;
	}

	void TypeInfo::RegisterMemberInternal(const char* name, const TypeInfo* memberType, uintptr_t offset)
	{
		if (const Member* existing = GetMember(name))
		{
			DD_ASSERT(existing->Type() == memberType);
			DD_ASSERT(existing->Offset() == offset);
			return;
		}

		Member& member = m_members.Allocate();
		member.m_name = name;
		member.m_typeInfo = memberType;
		member.m_parent = this;
		member.m_offset = offset;
	}

	void TypeInfo::RegisterDefaultTypes()
	{
		DD_ASSERT(!sm_defaultsRegistered);

		sm_defaultsRegistered = true;

		// integers
		DD_REGISTER_POD(int);
		DD_REGISTER_POD(char);
		DD_REGISTER_POD(int16);
		DD_REGISTER_POD(int64);

		// unsigned integers
		DD_REGISTER_POD(uint);
		DD_REGISTER_POD(byte);
		DD_REGISTER_POD(uint16);
		DD_REGISTER_POD(uint64);

		DD_REGISTER_POD(float);
		DD_REGISTER_POD(double);

		DD_REGISTER_POD(bool);

		DD_REGISTER_CLASS(dd::String);

		DD_REGISTER_CLASS(dd::String8);
		DD_REGISTER_PARENT(dd::String8, dd::String);

		DD_REGISTER_CLASS(dd::String16);
		DD_REGISTER_PARENT(dd::String16, dd::String);

		DD_REGISTER_CLASS(dd::String32);
		DD_REGISTER_PARENT(dd::String32, dd::String);

		DD_REGISTER_CLASS(dd::String64);
		DD_REGISTER_PARENT(dd::String64, dd::String);

		DD_REGISTER_CLASS(dd::String128);
		DD_REGISTER_PARENT(dd::String128, dd::String);

		DD_REGISTER_CLASS(dd::String256);
		DD_REGISTER_PARENT(dd::String256, dd::String);

		DD_REGISTER_CLASS(dd::SharedString);

		TypeInfo* vec2Type = DD_REGISTER_CLASS(glm::vec2);
		vec2Type->RegisterMember<glm::vec2, float, &glm::vec2::x>("x");
		vec2Type->RegisterMember<glm::vec2, float, &glm::vec2::y>("y");

		TypeInfo* ivec2Type = DD_REGISTER_CLASS(glm::ivec2);
		ivec2Type->RegisterMember<glm::ivec2, int, &glm::ivec2::x>("x");
		ivec2Type->RegisterMember<glm::ivec2, int, &glm::ivec2::y>("y");

		TypeInfo* vec3Type = DD_REGISTER_CLASS(glm::vec3);
		vec3Type->RegisterMember<glm::vec3, float, &glm::vec3::x>("x");
		vec3Type->RegisterMember<glm::vec3, float, &glm::vec3::y>("y");
		vec3Type->RegisterMember<glm::vec3, float, &glm::vec3::z>("z");

		TypeInfo* vec4Type = DD_REGISTER_CLASS(glm::vec4);
		vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::x>("x");
		vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::y>("y");
		vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::z>("z");
		vec4Type->RegisterMember<glm::vec4, float, &glm::vec4::w>("w");

		TypeInfo* quatType = DD_REGISTER_CLASS(glm::quat);
		quatType->RegisterMember<glm::quat, float, &glm::quat::x>("x");
		quatType->RegisterMember<glm::quat, float, &glm::quat::y>("y");
		quatType->RegisterMember<glm::quat, float, &glm::quat::z>("z");
		quatType->RegisterMember<glm::quat, float, &glm::quat::w>("w");

		TypeInfo* mat3Type = DD_REGISTER_CLASS(glm::mat3);
		mat3Type->RegisterMemberInternal("x", vec3Type, 0);
		mat3Type->RegisterMemberInternal("y", vec3Type, (uintptr_t) vec3Type->Size());
		mat3Type->RegisterMemberInternal("z", vec3Type, (uintptr_t) vec3Type->Size() * 2);

		TypeInfo* mat4Type = DD_REGISTER_CLASS(glm::mat4);
		mat4Type->RegisterMemberInternal("x", vec4Type, 0);
		mat4Type->RegisterMemberInternal("y", vec4Type, (uintptr_t) vec4Type->Size());
		mat4Type->RegisterMemberInternal("z", vec4Type, (uintptr_t) vec4Type->Size() * 2);
		mat4Type->RegisterMemberInternal("w", vec4Type, (uintptr_t) vec4Type->Size() * 3);
	}

	void TypeInfo::QueueRegistration(std::function<void()> fn)
	{
		sm_registrations->push_back(fn);
	}

	void TypeInfo::RegisterQueuedTypes()
	{
		dd::TypeInfo::RegisterDefaultTypes();

		for (auto fn : *sm_registrations)
		{
			fn();
		}
	}
}