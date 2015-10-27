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
		String32 Name;
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
		inline const String& Name() const { return m_name; }

		inline bool IsPOD() const { return m_members.Size() == 0; }

		void* (*New)();
		void (*Copy)( void* data, const void* src );
		void (*Delete)( void* data );
		void (*NewCopy)( void* *dest, const void* src );
		void (*PlacementNew)( void* data );
		void (*PlacementDelete)( void* data );
		void (*PlacementCopy)( void* data, const void* src );

		inline bool IsRegistered() const { return m_size != 0; }

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

		template <typename T>
		void RegisterScriptObject( const char* name );

		template <typename T>
		void RegisterScriptStruct( const char* name );

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

		template <typename T>
		static TypeInfo* AccessType();

		bool HasCustomSerializers() const;
		void SetCustomSerializers( SerializeFn serializer, DeserializeFn deserializer );

		SerializeFn SerializeCustom;
		DeserializeFn DeserializeCustom;

		static void RegisterDefaultTypes();

	private:
		uint m_size;
		String32 m_name;
		const TypeInfo* m_containedType;
		bool m_scriptObject;

		Vector<Member> m_members;
		Vector<Method> m_methods;

		static bool sm_defaultsRegistered;
		static DenseMap<String32, TypeInfo*> sm_typeMap;
	};

	#include "TypeInfo.inl"
};
