//
// AngelScriptEngine.inl - Inline functions for AngelScriptEngine.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

template <typename T>
T* Factory()
{
	T* ptr = new T();
	ptr->m_refCount.Init( ptr, GET_TYPE( T ) );
	return ptr;
}

template <typename FnType>
void dd::AngelScriptEngine::RegisterMethod( const char* method_name, const Function& method, FnType fnPtr )
{
	class DummyClass {};
	const int METHOD_SIZE = sizeof(void (DummyClass::*)());

	String256 signature( GetFunctionSignatureString( method_name, method ) );

	String64 className( ReplacePointer( method.Signature()->GetContext()->Name().c_str() ) );

	int res = m_engine->RegisterObjectMethod( className.c_str(), signature.c_str(), asSMethodPtr<METHOD_SIZE>::Convert( fnPtr ), asCALL_THISCALL );
	DD_ASSERT( res >= 0, "Failed to register method \'%s\' for class \'%s\'!", signature.c_str(), className.c_str() );
}

template <typename T>
void dd::AngelScriptEngine::RegisterObject( const char* className )
{
	String64 objType( ReplacePointer( className ) );

	int res = m_engine->RegisterObjectType( objType.c_str(), 0, asOBJ_REF );
	DD_ASSERT( res >= 0 );

	String32 strSig( objType );
	strSig += "@ Factory()";

	res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_FACTORY, strSig.c_str(), asFUNCTION( Factory<T> ), asCALL_CDECL );
	DD_ASSERT( res >= 0 );

	res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_ADDREF, "void Increment()", asMETHOD( RefCounter, Increment ), asCALL_THISCALL );
	DD_ASSERT( res >= 0 );

	res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_RELEASE, "void Decrement()", asMETHOD( RefCounter, Decrement ), asCALL_THISCALL );
	DD_ASSERT( res >= 0 );
}

template<typename T>
void Construct( T* memory )
{
	new (memory) T();
}

template<typename T>
void CopyConstruct( const T& other, T* self )
{
	new (self) T( other );
}

template<typename T>
void Destruct( T* memory )
{
	memory->~T();
}

template <typename T>
void dd::AngelScriptEngine::RegisterStruct( const char* className )
{
	String64 objType( ReplacePointer( className ) );

	int res = m_engine->RegisterObjectType( objType.c_str(), sizeof( T ), asOBJ_VALUE | asGetTypeTraits<T>() );
	DD_ASSERT( res >= 0, "Failed to register struct '%s'!", className );

	res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_CONSTRUCT, "void Construct()", asFUNCTION( Construct<T> ), asCALL_CDECL_OBJLAST );
	DD_ASSERT( res >= 0 );

	String128 copyConstructSig;
	copyConstructSig += "void CopyConstruct(const ";
	copyConstructSig += objType.c_str();
	copyConstructSig += "& in)";

	res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_CONSTRUCT, copyConstructSig.c_str(), asFUNCTION( CopyConstruct<T> ), asCALL_CDECL_OBJLAST );
	DD_ASSERT( res >= 0 );

	res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_DESTRUCT, "void Destruct()", asFUNCTION( Destruct<T> ), asCALL_CDECL_OBJLAST );
	DD_ASSERT( res >= 0 );

	String128 opAssignSig;
	opAssignSig += objType.c_str();
	opAssignSig += "& opAssign(const ";
	opAssignSig += objType.c_str();
	opAssignSig += "& in)";

	res = m_engine->RegisterObjectMethod( objType.c_str(), opAssignSig.c_str(), asMETHODPR( T, operator=, (const T&), T& ), asCALL_THISCALL ); 
	DD_ASSERT( res >= 0 );
}

template <typename FnType>
void dd::AngelScriptEngine::RegisterGlobalFunction( const char* name, const Function& function, FnType ptr )
{
	String256 signature = GetFunctionSignatureString( name, function );

	int res = m_engine->RegisterGlobalFunction( signature.c_str(), asFUNCTION( ptr ), asCALL_CDECL );
	DD_ASSERT( res >= 0, "Failed to register global function \'%s\'!", signature.c_str() );
}

template <typename T, T& Variable>
void AngelScriptEngine::RegisterGlobalVariable( const char* name )
{
	const TypeInfo* typeInfo = GET_TYPE( T );
	DD_ASSERT( typeInfo != nullptr );

	String128 signature;
	signature += ReplacePointer( typeInfo->Name().c_str() );
	signature += " ";
	signature += name;

	int res = m_engine->RegisterGlobalProperty( signature.c_str(), &Variable );
	DD_ASSERT( res >= 0 );
}