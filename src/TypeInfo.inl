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
	ASSERT( sm_defaultsRegistered );

	TypeInfo* typeInfo = const_cast<TypeInfo*>(GetType<T>());
	if( typeInfo->IsRegistered() )
		return typeInfo;

	typeInfo->Init( name, sizeof( T ) );

	typedef std::conditional<HasDefaultCtor<T>::value, T, EmptyType<T>>::type new_type;
	typeInfo->New = SetFunc<HasDefaultCtor<T>::value, void*(*)(), &dd::New<new_type>>::Get();
	typeInfo->PlacementNew = SetFunc<HasDefaultCtor<T>::value, void( *)(void*), &dd::PlacementNew<new_type>>::Get();
	typeInfo->Copy = SetFunc<HasCopyCtor<T>::value, void( *)(void*, const void*), &dd::Copy<new_type>>::Get();
	typeInfo->Delete = dd::Delete<T>;
	typeInfo->PlacementCopy = SetFunc<HasCopyCtor<T>::value, void( *)(void*, const void*), &dd::PlacementCopy<new_type>>::Get();
	typeInfo->PlacementDelete = dd::PlacementDelete<T>;
	typeInfo->NewCopy = SetFunc<HasCopyCtor<T>::value, void( *)(void**, const void*), &dd::NewCopy<new_type>>::Get();
	typeInfo->SerializeCustom = nullptr;
	typeInfo->DeserializeCustom = nullptr;

	sm_typeMap.Add( name, typeInfo );

	T::RegisterMembers();

	return typeInfo;
}

template <typename T>
const TypeInfo* TypeInfo::RegisterPOD( const char* name )
{
	ASSERT( sm_defaultsRegistered );

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

	sm_typeMap.Add( name, typeInfo );

	RegisterContainer<Vector<T>>( "Vector", typeInfo );

	return typeInfo;
}

template<typename T>
const TypeInfo* TypeInfo::RegisterContainer( const char* container, const TypeInfo* containing )
{
	ASSERT( sm_defaultsRegistered );

	TypeInfo* typeInfo = const_cast<TypeInfo*>(GetType<T>());
	if( typeInfo->IsRegistered() )
		return typeInfo;

	String32 finalName( container );
	finalName += "<";
	finalName += containing->Name();
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

	sm_typeMap.Add( finalName, typeInfo );

	return typeInfo;
}

template <typename FnType>
void TypeInfo::RegisterMethod( Function f, FnType fn, const char* name )
{
	ASSERT( sm_defaultsRegistered );

	Method& m = m_methods.Allocate();
	m.Name = name;
	m.Function = f;

	// don't register with script if this isn't a script object
	if( m_scriptObject )
	{
		const FunctionSignature* sig = f.Signature();
		String128 signature;

		if( sig->GetRet() != nullptr )
			signature += sig->GetRet()->GetNameWithoutNamespace();
		else
			signature += "void";

		signature += " ";
		signature += name;

		signature += "(";

		uint argCount = sig->ArgCount();
		for( uint i = 0; i < argCount; ++i )
		{
			signature += sig->GetArg( i )->GetNameWithoutNamespace();

			if( i < (argCount - 1) )
				signature += ",";
		}

		signature += ")";

		ScriptEngine::GetInstance()->RegisterMethod( sig->GetContext()->GetNameWithoutNamespace(), signature, fn );
	}
}

template <typename T>
void TypeInfo::RegisterScriptObject( const char* name )
{
	m_scriptObject = true;

	ScriptEngine::GetInstance()->RegisterObject<T>( GetNameWithoutNamespace() );
}