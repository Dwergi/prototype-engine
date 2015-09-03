//
// TypeInfo.h - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
// Heavily influenced by Randy Gaul (http://RandyGaul.net)
//

#pragma once

#include "RemoveQualifiers.h"
#include "TypeInfoHelpers.h"

namespace dd
{
	class Variable;
	class TypeInfo;

/*
	typedef void (*SerializeCB)( File&, Variable );
	typedef void (*DeserializeCB)( File&, Variable );*/

	class Member
	{
	public:
		const TypeInfo* Type() const;
		uint Offset() const;
		const String& Name() const;
		Property GetProperty() const;

	private:
		String m_name;
		uint m_offset;
		const TypeInfo* m_typeInfo;
		Property m_property;

		friend class TypeInfo;
	};

	class TypeInfo : public AutoList<TypeInfo>
	{
	public:
		TypeInfo();
		void Init( const char* name, unsigned size );
		void AddMember( const TypeInfo* typeInfo, const char* name, unsigned offset );
		const Member* GetMember( const char* memberName ) const;
		void AddProperty( const char* memberName, const Property& prop );

		unsigned Size() const;
		const char* Name() const;
		inline bool IsPOD() const { return m_isPOD; };

		void* (*New)();
		void (*Copy)( void* data, const void* src );
		void (*Delete)( void* data );
		void (*NewCopy)( void* *dest, const void* src );
		void (*PlacementNew)( void* data );
		void (*PlacementDelete)( void* data );
		void (*PlacementCopy)( void* data, const void* src );

		template<typename T>
		static const TypeInfo* RegisterType( uint size, const char* name, bool isPOD );

		template <typename T>
		static const TypeInfo* GetType();
		static const TypeInfo* GetType( const char* typeName );
		static const TypeInfo* GetType( const String& typeName );

		/*
		void SetSerializer( SerializeCB cb );
		void SetDeserializer( DeserializeCB cb );
		void Serialize( File& file, Variable var ) const;
		void Deserialize( File& file, Variable var ) const;
		*/

	private:
		unsigned m_size;
		String m_name;
		Vector<Member> m_members;
		bool m_isPOD;

		static DenseMap<String, TypeInfo*> sm_typeMap;

		/*
		SerializeCB m_serialize;
		DeserializeCB m_deserialize;

		ToLuaCB m_toLua;
		FromLuaCB m_fromLua;
		const char* m_metatable;
		Array<luaL_Reg> m_luaMethods;
		*/

		
/*

		friend class TextSerializer;
		friend class Engine;*/
	};

	template<typename T>
	const TypeInfo* TypeInfo::GetType()
	{
		static TypeInfo instance;
		return &instance;
	}

	template <typename T>
	const TypeInfo* TypeInfo::RegisterType( uint size, const char* name, bool isPOD )
	{
		TypeInfo* typeInfo = const_cast<TypeInfo*>( GetType<T>() );
		typeInfo->Init( name, size );
		typeInfo->m_isPOD = isPOD;

		if( isPOD )
		{
			typeInfo->New = PODNew<T>;
			typeInfo->Copy = PODCopy<T>;
			typeInfo->NewCopy = PODNewCopy<T>;
			typeInfo->Delete = PODDelete<T>;
			typeInfo->PlacementNew = PODPlacementNew<T>;
			typeInfo->PlacementDelete = PODPlacementDelete<T>;
			typeInfo->PlacementCopy = PODPlacementCopy<T>;
		}
		else
		{
			typedef std::conditional<HasDefaultCtor<T>::value, T, EmptyType<T>>::type new_type;
			typeInfo->New = SetFunc<HasDefaultCtor<T>::value, void*(*)(), &dd::New<new_type>>::Get();
			typeInfo->PlacementNew = SetFunc<HasDefaultCtor<T>::value, void (*)( void* ), &dd::PlacementNew<new_type>>::Get();
			typeInfo->Copy = SetFunc<HasCopyCtor<T>::value, void(*)( void*, const void* ), &dd::Copy<new_type>>::Get();
			typeInfo->Delete = dd::Delete<T>;
			typeInfo->PlacementCopy = SetFunc<HasCopyCtor<T>::value, void(*)( void*, const void* ), &dd::PlacementCopy<new_type>>::Get();
			typeInfo->PlacementDelete = dd::PlacementDelete<T>;
			typeInfo->NewCopy = SetFunc<HasCopyCtor<T>::value, void(*)( void**, const void* ), &dd::NewCopy<new_type>>::Get();
		}

		sm_typeMap.Add( name, typeInfo );
		return typeInfo;
	}

	static void RegisterDefaultTypes();
};
