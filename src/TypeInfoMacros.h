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
	dd::TypeInfo::RegisterType<dd::RemoveQualifiers<TypeName>::type>( sizeof( TypeName ), #TypeName )

#define REGISTER_POD( TypeName, Format ) \
	dd::TypeInfo::RegisterPOD<dd::RemoveQualifiers<TypeName>::type>( sizeof( TypeName ), #TypeName, Format )

#define REGISTER_POINTER( TypeName ) \
	dd::TypeInfo::RegisterPOD<dd::RemoveQualifiers<TypeName>::type>( sizeof( TypeName ), #TypeName )

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

#define MEMBER( MemberName ) \
	typeInfo->AddMember( GET_TYPE_OF( instance.MemberName ), #MemberName, (uint(&instance.MemberName) - uint(&instance)) )

#define METHOD( MethodName ) \
	typeInfo->AddMethod( FUNCTION( MethodName ), &MethodName, #MethodName );

#define END_MEMBERS }

#define NO_MEMBERS( TypeName ) \
	static void RegisterMembers() { GET_TYPE( TypeName ); }

#define SET_SERIALIZERS( TypeName, Serializer, Deserializer ) \
	const_cast<dd::TypeInfo*>( GET_TYPE( TypeName ) )->SetCustomSerializers( Serializer, Deserializer )

#define FUNCTION( FN ) \
	dd::BuildFunction<decltype( &FN ), &FN>( &FN )