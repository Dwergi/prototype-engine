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
		typedef std::conditional<std::is_default_constructible<T>::value, T, EmptyType<T>>::type DefaultCtorType;
		typedef std::conditional<std::is_copy_constructible<T>::value, T, EmptyType<T>>::type CopyCtorType;
		typedef std::conditional<std::is_copy_assignable<T>::value, T, EmptyType<T>>::type CopyAssignType;

		New = SetFunc<std::is_default_constructible<T>::value, void* (*)(), &dd::New<DefaultCtorType>>::Get();
		PlacementNew = SetFunc<std::is_default_constructible<T>::value, void( *)(void*), &dd::PlacementNew<DefaultCtorType>>::Get();
		Copy = SetFunc<std::is_copy_assignable<T>::value, void( *)(void*, const void*), &dd::Copy<CopyAssignType>>::Get();
		Delete = dd::Delete<T>;
		PlacementCopy = SetFunc<std::is_copy_constructible<T>::value, void( *)(void*, const void*), &dd::PlacementCopy<CopyCtorType>>::Get();
		PlacementDelete = dd::PlacementDelete<T>;
		NewCopy = SetFunc<std::is_copy_constructible<T>::value, void( *)(void**, const void*), &dd::NewCopy<CopyCtorType>>::Get();
	}

	template <typename T>
	const TypeInfo* TypeInfo::RegisterType( const char* name )
	{
		DD_ASSERT( sm_defaultsRegistered );

		TypeInfo* typeInfo = const_cast<TypeInfo*>(GetType<T>());
		if( typeInfo->IsRegistered() )
			return typeInfo;

		typeInfo->Init( name, sizeof( T ) );

		typeInfo->RegisterFunctions<T>();

		typeInfo->SerializeCustom = nullptr;
		typeInfo->DeserializeCustom = nullptr;

		sm_typeMap.Add( SharedString( name ), typeInfo );

		// register pointer and references as well
		{
			String64 ptrName( name );
			ptrName += "*";
			TypeInfo* ptrInfo = AccessType<T*>();
			ptrInfo->Init( ptrName.c_str(), sizeof( T* ) );
			sm_typeMap.Add( SharedString( ptrName ), ptrInfo );

			String64 refName( name );
			refName += "&";
			TypeInfo* refInfo = AccessType<T&>();
			refInfo->Init( refName.c_str(), sizeof( T& ) );
			sm_typeMap.Add( SharedString( refName ), refInfo );
		}

		T::RegisterMembers( typeInfo );

		return typeInfo;
	}

	template <typename T>
	const TypeInfo* TypeInfo::RegisterPOD( const char* name )
	{
		DD_ASSERT( sm_defaultsRegistered );

		TypeInfo* typeInfo = const_cast<TypeInfo*>(GetType<T>());
		if( typeInfo->IsRegistered() )
			return typeInfo;

		typeInfo->Init( name, sizeof( T ) );

		typeInfo->New = PODNew<T>;
		typeInfo->Copy = PODCopy<T>;
		typeInfo->NewCopy = PODNewCopy<T>;
		typeInfo->Delete = PODDelete<T>;
		typeInfo->PlacementNew = PODPlacementNew<T>;
		typeInfo->PlacementDelete = PODPlacementDelete<T>;
		typeInfo->PlacementCopy = PODPlacementCopy<T>;

		typeInfo->SerializeCustom = dd::Serialize::SerializePOD<T>;
		typeInfo->DeserializeCustom = dd::Serialize::DeserializePOD<T>;

		sm_typeMap.Add( SharedString( name ), typeInfo );
		
		RegisterContainer<Vector<T>>( "dd::Vector", typeInfo );

		return typeInfo;
	}

	template <typename T>
	const TypeInfo* TypeInfo::RegisterContainer( const char* container, const TypeInfo* containing )
	{
		DD_ASSERT( sm_defaultsRegistered );

		TypeInfo* typeInfo = const_cast<TypeInfo*>(GetType<T>());
		if( typeInfo->IsRegistered() )
			return typeInfo;

		String64 finalName( container );
		finalName += "<";
		finalName += containing->Name().c_str();
		finalName += ">";

		typeInfo->Init( finalName.c_str(), sizeof( T ) );
		typeInfo->m_containedType = containing;

		typeInfo->RegisterFunctions<T>();

		typeInfo->SerializeCustom = dd::Serialize::SerializeContainer<T>;
		typeInfo->DeserializeCustom = dd::Serialize::DeserializeContainer<T>;

		sm_typeMap.Add( SharedString( finalName ), typeInfo );

		return typeInfo;
	}

	template <typename T>
	void TypeInfo::RegisterParentType()
	{
		const TypeInfo* parent = GET_TYPE( T );
		DD_ASSERT( parent->IsRegistered() );

		m_parentType = parent;

		T::RegisterMembers( this );
	}

	template <typename FnType, FnType Fn>
	void TypeInfo::RegisterMethod( const char* name )
	{
		DD_ASSERT( sm_defaultsRegistered );
		DD_ASSERT( IsRegistered() );

		Method& m = m_methods.Allocate();
		m.Name = name;
		m.Function = dd::BuildFunction<FnType, Fn>( Fn );

		ScriptEngine* scriptEngine = Services::GetPtr<ScriptEngine>();

		// don't register with script if this isn't a script object
		if( m_scriptObject && scriptEngine != nullptr )
		{
			scriptEngine->RegisterFunction<FnType, Fn>( name );
		}
	}
	
	template <typename TClass, typename TProp, TProp TClass::* MemberPtr>
	void TypeInfo::RegisterMember( const char* name )
	{
		const TypeInfo* typeInfo = TypeInfo::GetType<TProp>();

		DD_ASSERT( sm_defaultsRegistered );
		DD_ASSERT( typeInfo->IsRegistered() );

		Member& member = m_members.Allocate();
		member.m_name = name;
		member.m_typeInfo = typeInfo;
		member.m_parent = this;
		member.m_offset = (uint64) (&(((TClass*) nullptr)->*MemberPtr));

		ScriptEngine* scriptEngine = Services::GetPtr<ScriptEngine>();
		DD_ASSERT( scriptEngine != nullptr );

		if( m_scriptObject && scriptEngine != nullptr )
		{
			scriptEngine->RegisterMember<TClass, TProp, MemberPtr>( name, this );
		}
	}

	template <typename T, bool byValue>
	void TypeInfo::RegisterScriptType()
	{
		ScriptEngine* scriptEngine = Services::GetPtr<ScriptEngine>();
		DD_ASSERT( scriptEngine != nullptr );

		if( scriptEngine != nullptr )
		{
			m_scriptObject = true;

			scriptEngine->RegisterType<T, byValue>();
		}
	}
}