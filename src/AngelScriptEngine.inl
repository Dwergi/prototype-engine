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

	String64 className( GetWithoutNamespace( method.Signature()->GetContext()->Name().c_str() ) );

	int res = m_engine->RegisterObjectMethod( className.c_str(), signature.c_str(), asSMethodPtr<METHOD_SIZE>::Convert( fnPtr ), asCALL_THISCALL );
	ASSERT( res >= 0, "Failed to register method \'%s\' for class \'%s\'!", signature.c_str(), className.c_str() );
}

template <typename ObjType>
void dd::AngelScriptEngine::RegisterObject( const char* className )
{
	String64 objType( GetWithoutNamespace( className ) );

	int res = m_engine->RegisterObjectType( objType.c_str(), 0, asOBJ_REF );
	ASSERT( res >= 0 );

	String32 strSig = objType;
	strSig += "@ Factory()";

	res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_FACTORY, strSig.c_str(), asFUNCTION( Factory<ObjType> ), asCALL_CDECL );
	ASSERT( res >= 0 );

	res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_ADDREF, "void Increment()", asMETHOD( RefCounter, Increment ), asCALL_THISCALL );
	ASSERT( res >= 0 );

	res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_RELEASE, "void Decrement()", asMETHOD( RefCounter, Decrement ), asCALL_THISCALL );
	ASSERT( res >= 0 );
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

template <typename ObjType>
void dd::AngelScriptEngine::RegisterStruct( const char* className )
{
	String64 objType( GetWithoutNamespace( className ) );

	int res = m_engine->RegisterObjectType( objType.c_str(), sizeof( ObjType ), asOBJ_VALUE | asGetTypeTraits<ObjType>() );
	ASSERT( res >= 0, "Failed to register struct '%s'!", className );

	res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_CONSTRUCT, "void Construct()", asFUNCTION( Construct<ObjType> ), asCALL_CDECL_OBJLAST );
	ASSERT( res >= 0 );

	String128 copyConstructSig;
	copyConstructSig += "void CopyConstruct(const ";
	copyConstructSig += objType.c_str();
	copyConstructSig += "& in)";

	res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_CONSTRUCT, copyConstructSig.c_str(), asFUNCTION( CopyConstruct<ObjType> ), asCALL_CDECL_OBJLAST );
	ASSERT( res >= 0 );

	res = m_engine->RegisterObjectBehaviour( objType.c_str(), asBEHAVE_DESTRUCT, "void Destruct()", asFUNCTION( Destruct<ObjType> ), asCALL_CDECL_OBJLAST );
	ASSERT( res >= 0 );

	String128 opAssignSig;
	opAssignSig += objType.c_str();
	opAssignSig += "& opAssign(const ";
	opAssignSig += objType.c_str();
	opAssignSig += "& in)";

	res = m_engine->RegisterObjectMethod( objType.c_str(), opAssignSig.c_str(), asMETHODPR( ObjType, operator=, (const ObjType&), ObjType& ), asCALL_THISCALL ); 
	ASSERT( res >= 0 );
}

template <typename FnType>
void dd::AngelScriptEngine::RegisterGlobalFunction( const char* name, const Function& function, FnType ptr, const char* explicit_sig )
{
	String256 signature;

	if( explicit_sig != nullptr )
		signature = explicit_sig;
	else 
		signature = GetFunctionSignatureString( name, function );

	int res = m_engine->RegisterGlobalFunction( signature.c_str(), asFUNCTION( ptr ), asCALL_CDECL );
	ASSERT( res >= 0, "Failed to register global function \'%s\'!", signature.c_str() );
}