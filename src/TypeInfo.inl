//
// TypeInfo.inl - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// October 11th 2015
// Heavily influenced by Randy Gaul (http://RandyGaul.net)
//

template<typename T>
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

template<typename T>
const TypeInfo* TypeInfo::RegisterType( const char* name )
{
	DD_ASSERT( sm_defaultsRegistered );

	TypeInfo* typeInfo = const_cast<TypeInfo*>(GetType<T>());
	if( typeInfo->IsRegistered() )
		return typeInfo;

	typeInfo->Init( name, sizeof( T ) );

	typedef std::conditional<HasDefaultCtor<T>::value, T, EmptyType<T>>::type new_type;
	typeInfo->New = SetFunc<HasDefaultCtor<T>::value, void*(*)(), &dd::New<new_type>>::Get();
	typeInfo->PlacementNew = SetFunc<HasDefaultCtor<T>::value, void(*)(void*), &dd::PlacementNew<new_type>>::Get();
	typeInfo->Copy = SetFunc<HasCopyCtor<T>::value, void(*)(void*, const void*), &dd::Copy<new_type>>::Get();
	typeInfo->Delete = dd::Delete<T>;
	typeInfo->PlacementCopy = SetFunc<HasCopyCtor<T>::value, void(*)(void*, const void*), &dd::PlacementCopy<new_type>>::Get();
	typeInfo->PlacementDelete = dd::PlacementDelete<T>;
	typeInfo->NewCopy = SetFunc<HasCopyCtor<T>::value, void(*)(void**, const void*), &dd::NewCopy<new_type>>::Get();

	typeInfo->SerializeCustom = nullptr;
	typeInfo->DeserializeCustom = nullptr;

	sm_typeMap.Add( SharedString( name ), typeInfo );

	// register pointer and references as well
	{
		String64 ptrName( name );
		ptrName += "*";
		TypeInfo* ptrInfo = const_cast<TypeInfo*>(GetType<T*>());
		ptrInfo->Init( ptrName.c_str(), sizeof( T* ) );
		sm_typeMap.Add( SharedString( ptrName ), ptrInfo );

		String64 refName( name );
		refName += "&";
		TypeInfo* refInfo = const_cast<TypeInfo*>(GetType<T&>());
		refInfo->Init( refName.c_str(), sizeof( T& ) );
		sm_typeMap.Add( SharedString( refName ), refInfo );
	}

	T::RegisterMembers();

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

template<typename T>
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

	typedef std::conditional<HasDefaultCtor<T>::value, T, EmptyType<T>>::type new_type;
	typeInfo->New = SetFunc<HasDefaultCtor<T>::value, void*(*)(), &dd::New<new_type>>::Get();
	typeInfo->PlacementNew = SetFunc<HasDefaultCtor<T>::value, void( *)(void*), &dd::PlacementNew<new_type>>::Get();
	typeInfo->Copy = SetFunc<HasCopyCtor<T>::value, void( *)(void*, const void*), &dd::Copy<new_type>>::Get();
	typeInfo->Delete = dd::Delete<T>;
	typeInfo->PlacementCopy = SetFunc<HasCopyCtor<T>::value, void( *)(void*, const void*), &dd::PlacementCopy<new_type>>::Get();
	typeInfo->PlacementDelete = dd::PlacementDelete<T>;
	typeInfo->NewCopy = SetFunc<HasCopyCtor<T>::value, void( *)(void**, const void*), &dd::NewCopy<new_type>>::Get();

	typeInfo->SerializeCustom = dd::Serialize::SerializeContainer<T>;
	typeInfo->DeserializeCustom = dd::Serialize::DeserializeContainer<T>;

	sm_typeMap.Add( SharedString( finalName ), typeInfo );

	return typeInfo;
}

template <typename FnType>
void TypeInfo::RegisterMethod( const Function& f, FnType fn, const char* name )
{
	DD_ASSERT( sm_defaultsRegistered );

	Method& m = m_methods.Allocate();
	m.Name = name;
	m.Function = f;

	AngelScriptEngine* script_engine = g_services.GetPtr<AngelScriptEngine>();

	// don't register with script if this isn't a script object
	if( m_scriptObject && script_engine != nullptr )
	{
		script_engine->RegisterMethod( name, f, fn );
	}
}

template <typename T>
void TypeInfo::RegisterScriptObject()
{
	AngelScriptEngine* script_engine = g_services.GetPtr<AngelScriptEngine>();
	DD_ASSERT( script_engine != nullptr );

	if( script_engine != nullptr )
	{
		m_scriptObject = true;

		script_engine->RegisterObject<T>( m_name.c_str() );
	}
}

template <typename T>
void TypeInfo::RegisterScriptStruct()
{
	AngelScriptEngine* script_engine = g_services.GetPtr<AngelScriptEngine>();
	DD_ASSERT( script_engine != nullptr );

	if( script_engine != nullptr )
	{
		m_scriptObject = true;

		script_engine->RegisterStruct<T>( m_name.c_str() );
	}
}