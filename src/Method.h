//
// Method.h - Represents a method that will be exposed to the scripting system
// Copyright (C) Sebastian Nordgren 
// August 11th 2015
//

#pragma once

#include "angelscript/include/angelscript.h"

#include "String_dd.h"

namespace dd
{
	class MethodBase
	{
	public:

		dd::String32 m_className;
		dd::String32 m_methodName;
		dd::String32 m_methodSignature;

		virtual int RegisterMethod( asIScriptEngine* engine ) = 0;
	};

	template< typename TClass, typename TMethod >
	class Method 
		: public MethodBase
	{
	public:

		typedef TClass ClassType;
		typedef TMethod MethodType;

		Method( MethodType ptr, const char* className, const char* methodName, const char* signature )
			: m_ptr( ptr )
		{
			m_className = className;
			m_methodName = methodName;
			m_methodSignature = signature;
		}

		virtual int RegisterMethod( asIScriptEngine* engine ) override
		{
			return engine->RegisterObjectMethod( m_className.c_str(), m_methodSignature.c_str(), asSMethodPtr<sizeof(MethodType)>::Convert( m_ptr ), asCALL_THISCALL );
		}

	private:

		MethodType m_ptr;	
	};

	//
	// TODO: Maybe look into making work?
	//
	class NullType
	{
	};

	template< typename T >
	class Parameter
	{
	private:
		template< typename T >
		struct IsNullHelper
		{
			enum { result = false };
		};

		template<>
		struct IsNullHelper< NullType >
		{
			enum { result = true };
		};

	public:

		static const bool IsNull = IsNullHelper<T>::result;
	};

	template< typename Method, typename P1 = NullType, typename P2 = NullType, typename P3 = NullType, typename P4 = NullType, typename P5 = NullType >
	class MethodCall
	{
	public:

		void Call( void* obj, void* p1 = nullptr, void* p2 = nullptr, void* p3 = nullptr, void* p4 = nullptr, void* p5 = nullptr )
		{
			/*if( Parameter<P1>::IsNull )
			{
				instance->*m_ptr();
			}
			else if( Parameter<P2>::IsNull )
			{
				instance->*m_ptr( *(P1*) p1 );
			}
			else if( Parameter<P3>::IsNull )
			{
				instance->*m_ptr( *(P1*) p1, *(P2*) p2 );
			}
			else if( Parameter<P4>::IsNull )
			{
				instance->*m_ptr( *(P1*) p1, *(P2*) p2, *(P3*) p3 );
			}
			else if( Parameter<P5>::IsNull )
			{
				instance->*m_ptr( *(P1*) p1, *(P2*) p2, *(P3*) p3, *(P4*) p4 );
			}
			else
			{
				instance->*m_ptr( *(P1*) p1, *(P2*) p2, *(P3*) p3, *(P4*) p4, *(P5*) p5 );
			}*/
		}
	};
}