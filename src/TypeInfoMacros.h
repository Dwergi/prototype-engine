//
// TypeInfoMacros.h - Utility macros to make it less painful to use the TypeInfo system.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
// Heavily inspired by Randy Gaul (http://RandyGaul.net)
//

#pragma once

#include "RemoveQualifiers.h"

// Introspection macros
#define REGISTER_TYPE( TypeName ) \
	dd::TypeInfo::RegisterType<dd::RemoveQualifiers<TypeName>::type>( #TypeName )

#define REGISTER_POD( TypeName ) \
	dd::TypeInfo::RegisterPOD<dd::RemoveQualifiers<TypeName>::type>( #TypeName )

#define REGISTER_CONTAINER( Container, Containing ) \
	dd::TypeInfo::RegisterContainer<dd::RemoveQualifiers<Container<Containing>>::type>( #Container, GET_TYPE( Containing ) )

#define GET_TYPE( TypeName ) \
	dd::TypeInfo::GetType<dd::RemoveQualifiers<TypeName>::type>()

#define GET_TYPE_OF( Object ) \
	dd::TypeInfo::GetType<dd::RemoveQualifiers<decltype( Object )>::type>()

#define GET_TYPE_OF_MEMBER( TypeName, MemberName ) \
	dd::TypeInfo::GetType<dd::RemoveQualifiers<dd::StripMemberness<decltype( &TypeName::MemberName )>::type>::type>()

#define OFFSET_OF( TypeName, MemberName ) \
	((unsigned int) (&((((TypeName*) nullptr))->MemberName)))

#define GET_TYPE_STR( NameString ) \
	dd::TypeInfo::GetType( NameString )

#define BEGIN_MEMBERS( TypeName ) \
	static void RegisterMembers() { TypeName instance; dd::TypeInfo* typeInfo = (dd::TypeInfo*) GET_TYPE( TypeName );

#define BEGIN_SCRIPT_OBJECT( TypeName ) \
	dd::RefCounter<TypeName> m_refCount; \
	BEGIN_MEMBERS( TypeName ) \
	typeInfo->RegisterScriptObject<TypeName>( #TypeName );

#define BEGIN_SCRIPT_STRUCT( TypeName ) \
	BEGIN_MEMBERS( TypeName ) \
	typeInfo->RegisterScriptStruct<TypeName>( #TypeName );

#define MEMBER( MemberName ) \
	typeInfo->RegisterMember( GET_TYPE_OF( instance.MemberName ), #MemberName, (uint) (reinterpret_cast<uint64>(&instance.MemberName) - reinterpret_cast<uint64>(&instance)) )

#define METHOD( MethodName ) \
	typeInfo->RegisterMethod( FUNCTION( MethodName ), &MethodName, #MethodName )

#define END_MEMBERS }

#define NO_MEMBERS( TypeName ) \
	static void RegisterMembers() {}

#define SET_SERIALIZERS( TypeName, Serializer, Deserializer ) \
	const_cast<dd::TypeInfo*>( GET_TYPE( TypeName ) )->SetCustomSerializers( Serializer, Deserializer )

#define FUNCTION( FN ) \
	dd::BuildFunction<decltype( &FN ), &FN>( &FN )