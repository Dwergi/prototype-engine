//
// TypeInfo.inl - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// October 11th 2015
// Heavily influenced by Randy Gaul (http://RandyGaul.net)
//

namespace dd
{
	template <typename T>
	const TypeInfo* TypeInfo::GetType()
	{
		static TypeInfo instance;
		return &instance;
	}

	template <typename T>
	TypeInfo* TypeInfo::AccessType()
	{
		return const_cast<TypeInfo*>(GetType<T>());
	}

	// Helper to register functions for the given type.
	template <typename T>
	void TypeInfo::RegisterFunctions()
	{
		if constexpr (std::is_default_constructible_v<T>)
		{
			New = &dd::New<T>;
		}
		Delete = dd::Delete<T>;

		if constexpr (std::is_copy_assignable_v<T>)
		{
			Copy = &dd::Copy<T>;
		}
		else if constexpr (std::is_copy_constructible_v<T>)
		{
			Copy = &dd::PlacementCopy<T>;
		}

		if constexpr (std::is_default_constructible_v<T>)
		{
			PlacementNew = &dd::PlacementNew<T>;
		}
		PlacementDelete = dd::PlacementDelete<T>;
	}

	template <typename T>
	TypeInfo* TypeInfo::RegisterType(const char* name)
	{
		TypeInfo* typeInfo = const_cast<TypeInfo*>(GetType<T>());
		if (typeInfo->IsRegistered())
			return typeInfo;

		typeInfo->m_typeKind = TypeKind::Class;

		typeInfo->Initialize(name, sizeof(T));

		typeInfo->RegisterFunctions<T>();

		sm_typeMap->insert(std::make_pair(String64(name), typeInfo));

		// register pointer and references as well
		{
			String64 ptrName(name);
			ptrName += "*";
			TypeInfo* ptrInfo = AccessType<T*>();
			ptrInfo->Initialize(ptrName.c_str(), sizeof(T*));
			sm_typeMap->insert(std::make_pair(String32(ptrName), ptrInfo));

			String64 refName(name);
			refName += "&";
			TypeInfo* refInfo = AccessType<T&>();
			refInfo->Initialize(refName.c_str(), sizeof(T&));
			sm_typeMap->insert(std::make_pair(String32(refName), refInfo));
		}

		return typeInfo;
	}

	template <typename T>
	TypeInfo* TypeInfo::RegisterPOD(const char* name)
	{
		TypeInfo* typeInfo = const_cast<TypeInfo*>(GetType<T>());
		if (typeInfo->IsRegistered())
			return typeInfo;

		typeInfo->m_typeKind = TypeKind::POD;

		typeInfo->Initialize(name, sizeof(T));

		typeInfo->New = PODNew<T>;
		typeInfo->Copy = PODCopy<T>;
		typeInfo->Delete = PODDelete<T>;
		typeInfo->PlacementNew = PODPlacementNew<T>;
		typeInfo->PlacementDelete = PODPlacementDelete<T>;

		sm_typeMap->insert(std::make_pair(String32(name), typeInfo));

		RegisterContainer<Vector<T>, T>("dd::Vector");

		return typeInfo;
	}

	template <typename TContainer, typename TItem>
	TypeInfo* TypeInfo::RegisterContainer(const char* container)
	{
		TypeInfo* typeInfo = const_cast<TypeInfo*>(GetType<TContainer>());
		if (typeInfo->IsRegistered())
		{
			return typeInfo;
		}

		const TypeInfo* itemType = GetType<TItem>();
		DD_ASSERT(itemType->IsRegistered());

		typeInfo->m_typeKind = TypeKind::Container;

		String64 finalName(container);
		finalName += "<";
		finalName += itemType->Name().c_str();
		finalName += ">";

		typeInfo->Initialize(finalName.c_str(), sizeof(TContainer));
		typeInfo->m_containedType = itemType;

		typeInfo->RegisterFunctions<TContainer>();

		typeInfo->ContainerSize = dd::ContainerSize<TContainer>;
		typeInfo->ElementAt = dd::ElementAt<TContainer>;
		typeInfo->InsertElement = dd::InsertElement<TContainer, TItem>;

		sm_typeMap->insert(std::make_pair(String32(finalName), typeInfo));

		return typeInfo;
	}

	template <typename TEnum>
	TypeInfo* TypeInfo::RegisterEnum(const char* name)
	{
		TypeInfo* typeInfo = const_cast<TypeInfo*>(GetType<TEnum>());
		if (typeInfo->IsRegistered())
		{
			return typeInfo;
		}

		typeInfo->m_typeKind = TypeKind::Enum;

		typeInfo->Initialize(name, sizeof(TEnum));

		typeInfo->New = PODNew<TEnum>;
		typeInfo->Copy = PODCopy<TEnum>;
		typeInfo->Delete = PODDelete<TEnum>;
		typeInfo->PlacementNew = PODPlacementNew<TEnum>;
		typeInfo->PlacementDelete = PODPlacementDelete<TEnum>;

		sm_typeMap->insert(std::make_pair(String32(name), typeInfo));

		return typeInfo;
	}

	template <typename T>
	void TypeInfo::RegisterParentType()
	{
		const TypeInfo* parent = DD_FIND_TYPE(T);
		DD_ASSERT(parent->IsRegistered());

		m_parentType = parent;

		T::RegisterMembers(this);
	}

	template <typename FnType, FnType Fn>
	void TypeInfo::RegisterMethod(const char* name)
	{
		DD_ASSERT(IsRegistered());

		Method& m = m_methods.Allocate();
		m.Name = name;
		m.Function = dd::BuildFunction<FnType, Fn>(Fn);
	}

	template <typename TClass, typename TProp, TProp TClass::* MemberPtr>
	void TypeInfo::RegisterMember(const char* name)
	{
		DD_ASSERT(IsRegistered());

		const TypeInfo* memberType = TypeInfo::GetType<TProp>();
		DD_ASSERT(memberType->IsRegistered());

		uintptr_t offset = (uintptr_t) (&(((TClass*) nullptr)->*MemberPtr));
		RegisterMemberInternal(name, memberType, offset);
	}

	template <typename T>
	void RegisterEnumOptions(dd::TypeInfo* typeInfo)
	{
		DD_ASSERT(false, "Enum options being registered for non-existent enum!");
	}

	template <typename T>
	void TypeInfo::RegisterEnumOption(T value, const char* name)
	{
		DD_ASSERT(IsRegistered());

		EnumOption& option = m_enumOptions.Allocate();
		option.Value = (int) value;
		option.Name = name;
	}
}