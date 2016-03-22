//
// TypeInfo.h - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
// Heavily influenced by Randy Gaul (http://RandyGaul.net)
//

#pragma once

namespace dd
{
	namespace Serialize
	{
		enum class Mode : uint;
	}

	class WriteStream;
	class ReadStream;

	typedef void (*SerializeFn)( Serialize::Mode mode, WriteStream& dst, Variable src );
	typedef void (*DeserializeFn)( Serialize::Mode mode, ReadStream& src, Variable dst );

	struct Method
	{
		SharedString Name;
		Function Function;
	};

	class TypeInfo : public AutoList<TypeInfo>
	{
	public:
		TypeInfo();
		void Init( const char* name, uint size );

		const Vector<Member>& Members() const { return m_members; }
		const Member* GetMember( const char* memberName ) const;

		const Function* GetMethod( const char* methodName ) const;

		inline uint Size() const { return m_size; }
		inline const SharedString& Name() const { return m_name; }
		inline const SharedString& Namespace() const { return m_namespace; }
		String128 FullTypeName() const;

		inline bool IsPOD() const { return m_members.Size() == 0; }
		inline bool IsRegistered() const { return m_size != 0; }

		bool IsDerivedFrom( const TypeInfo* type ) const;

		void* (*New)();
		void (*Copy)( void* data, const void* src );
		void (*Delete)( void* data );
		void (*NewCopy)( void* *dest, const void* src );
		void (*PlacementNew)( void* data );
		void (*PlacementDelete)( void* data );
		void (*PlacementCopy)( void* data, const void* src );

		//
		// Register a non-POD, non-container type (eg. a class).
		//
		template <typename T>
		static const TypeInfo* RegisterType( const char* name );

		template <typename T>
		static const TypeInfo* RegisterContainer( const char* container, const TypeInfo* containing );

		template <typename T>
		void RegisterMethod( const Function& f, T fn, const char* name );

		void RegisterMember( const TypeInfo* typeInfo, const char* name, uint offset );

		void RegisterParentType( const TypeInfo* typeInfo );

		template <typename T>
		void RegisterScriptObject();

		template <typename T>
		void RegisterScriptStruct();

		//
		// Register a POD type - these are the basic types like ints, floats and char*.
		// If it can be printed with a printf format string, it's POD.
		//
		template<typename T>
		static const TypeInfo* RegisterPOD( const char* name );

		template <typename T>
		static const TypeInfo* GetType();
		static const TypeInfo* GetType( const char* typeName );
		static const TypeInfo* GetType( const String& typeName );
		static const TypeInfo* GetType( const SharedString& typeName );

		template <typename T>
		static TypeInfo* AccessType();

		bool HasCustomSerializers() const;
		void SetCustomSerializers( SerializeFn serializer, DeserializeFn deserializer );

		SerializeFn SerializeCustom;
		DeserializeFn DeserializeCustom;

		static void RegisterDefaultTypes();

	private:
		uint m_size;
		SharedString m_namespace;
		SharedString m_name;
		bool m_scriptObject;

		const TypeInfo* m_parentType;
		const TypeInfo* m_containedType;
		
		Vector<Member> m_members;
		Vector<Method> m_methods;

		static bool sm_defaultsRegistered;
		static DenseMap<SharedString, TypeInfo*> sm_typeMap;
	};

	#include "TypeInfo.inl"
};
