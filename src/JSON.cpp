//
// JSON.cpp - Wrapper around JSON parsing for objects with TypeInfo.
// Copyright (C) Sebastian Nordgren 
// October 8th 2018
//

#include "PCH.h"
#include "JSON.h"

#include "File.h"

#include <nlohmann/json.hpp>

#include <iostream>

namespace dd
{
	static void WritePOD(nlohmann::json& json, Variable& var)
	{
		if (var.Type() == DD_FIND_TYPE(float))
		{
			json = var.GetValue<float>();
		}
		else if (var.Type() == DD_FIND_TYPE(int))
		{
			json = var.GetValue<int>();
		}
		else if (var.Type() == DD_FIND_TYPE(bool))
		{
			json = var.GetValue<bool>();
		}
	}

	static void WriteEnum(nlohmann::json& json, Variable& var)
	{
		int current = var.GetValue<int>();

		const Vector<EnumOption>& options = var.Type()->GetEnumOptions();

		String32 selected;
		for (const EnumOption& o : options)
		{
			if (current == o.Value)
			{
				selected = o.Name;
				break;
			}
		}

		json = selected.c_str();
	}

	static void WriteVariable(nlohmann::json& json, Variable& var);

	static void WriteContainer(nlohmann::json& json, Variable& var)
	{
		json = nlohmann::json::array();

		uint size = var.Type()->ContainerSize(var.Data());
		for (uint i = 0; i < size; ++i)
		{
			void* item = var.Type()->ElementAt(var.Data(), i);

			Variable item_var(var.Type()->ContainedType(), item);

			nlohmann::json item_json;
			WriteVariable(item_json, item_var);

			json.push_back(item_json);
		}
	}

	static void WriteClass(nlohmann::json& json, Variable& var)
	{
		json = nlohmann::json::object();

		for (const Member& member : var.Type()->Members())
		{
			Variable member_var(var, member);
			nlohmann::json& member_json = json[member.Name().c_str()] = nlohmann::json();

			WriteVariable(member_json, member_var);
		}
	}

	static void WriteVariable(nlohmann::json& json, Variable& var)
	{
		switch (var.Type()->GetTypeKind())
		{
			case TypeKind::POD:
				WritePOD(json, var);
				break;

			case TypeKind::Class:
				WriteClass(json, var);
				break;

			case TypeKind::Enum:
				WriteEnum(json, var);
				break;

			case TypeKind::Container:
				WriteContainer(json, var);
				break;
		}
	}

	static void ReadPOD(const nlohmann::json& json, Variable& var)
	{
		if (var.Type() == DD_FIND_TYPE(float))
		{
			DD_ASSERT(json.is_number_float());

			var.AccessValue<float>() = json.get<float>();
		}
		else if (var.Type() == DD_FIND_TYPE(int))
		{
			DD_ASSERT(json.is_number_integer());

			var.AccessValue<int>() = json.get<int>();
		}
		else if (var.Type() == DD_FIND_TYPE(bool))
		{
			DD_ASSERT(json.is_boolean());

			var.AccessValue<bool>() = json.get<bool>();
		}
	}

	static void ReadEnum(const nlohmann::json& json, Variable& var)
	{
		DD_ASSERT(json.is_string());

		std::string selected = json.get<std::string>();

		const Vector<EnumOption>& options = var.Type()->GetEnumOptions();
		for (const EnumOption& o : options)
		{
			if (o.Name == selected.c_str())
			{
				var.AccessValue<int>() = o.Value;
				break;
			}
		}
	}

	static void ReadVariable(const nlohmann::json& json, Variable& var);

	static void ReadContainer(const nlohmann::json& json, Variable& var)
	{
		DD_ASSERT(json.is_array());

		const TypeInfo* item_type = var.Type()->ContainedType();

		std::unique_ptr<byte[]> buffer(new byte[item_type->Size()]);

		for (uint i = 0; i < json.size(); ++i)
		{
			const nlohmann::json& item_json = json.at(i);

			item_type->PlacementNew(buffer.get());

			Variable item_var(item_type, buffer.get());
			var.Type()->InsertElement(var.Data(), i, buffer.get());
		}
	}

	static void ReadClass(const nlohmann::json& json, Variable& var)
	{
		DD_ASSERT(json.is_object());

		for (const Member& member : var.Type()->Members())
		{
			Variable member_var(var, member);
			const nlohmann::json& member_json = json.at(member.Name().c_str());

			ReadVariable(member_json, member_var);
		}
	}

	static void ReadVariable(const nlohmann::json& json, Variable& var)
	{
		switch (var.Type()->GetTypeKind())
		{
			case TypeKind::POD:
				ReadPOD(json, var);
				break;

			case TypeKind::Class:
				ReadClass(json, var);
				break;

			case TypeKind::Enum:
				ReadEnum(json, var);
				break;

			case TypeKind::Container:
				ReadContainer(json, var);
				break;
		}
	}

	void JSON::Write(Variable& var, nlohmann::json& out)
	{
		DD_ASSERT(var.Type()->GetTypeKind() == dd::TypeKind::Class);

		WriteClass(out, var);
	}

	void JSON::Read(Variable& var, const nlohmann::json& in)
	{
		DD_ASSERT(var.Type()->GetTypeKind() == dd::TypeKind::Class);

		ReadClass(in, var);
	}
}