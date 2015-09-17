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

	class TypeInfo : public AutoList<TypeInfo>
	{
	public:
		TypeInfo();
		void Init( const char* name, uint size );

		void AddMember( const TypeInfo* typeInfo, const char* name, uint offset );
		const Vector<Member>& GetMembers() const { return m_members; }
		const Member* GetMember( const char* memberName ) const;
		
		template <typename T>
		void AddMethod( Function f, T fn, const char* name );
		const Function* GetMethod( const char* methodName ) const;

		inline uint Size() const { return m_size; }
		inline const String& Name() const { return m_name; }
		inline const TypeInfo* ContainedType() const { return m_containedType; }

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
		template<typename T>
		static const TypeInfo* RegisterType( const char* name );

		template<typename T>
		static const TypeInfo* RegisterContainer( const char* container, const TypeInfo* containing );

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

	private:
		uint m_size;
		String32 m_name;
		const TypeInfo* m_containedType;

		struct Method
		{
			String32 Name;
			Function Function;
		};

		Vector<Member> m_members;
		Vector<Method> m_methods;

		static DenseMap<String32, TypeInfo*> sm_typeMap;
	};

	void RegisterDefaultTypes();

	template<typename T>
	const TypeInfo* TypeInfo::GetType()
	{
		static TypeInfo instance;
		return &instance;
	}

	template <typename T>
	TypeInfo* TypeInfo::AccessType()
	{
		return const_cast<TypeInfo*>( GetType<T>() );
	}

	template<typename T>
	const TypeInfo* TypeInfo::RegisterType( const char* name )
	{
		TypeInfo* typeInfo = const_cast<TypeInfo*>( GetType<T>() );
		if( typeInfo->IsRegistered() )
			return typeInfo;

		typeInfo->Init( name, sizeof( T ) );

		typedef std::conditional<HasDefaultCtor<T>::value, T, EmptyType<T>>::type new_type;
		typeInfo->New = SetFunc<HasDefaultCtor<T>::value, void*(*)(), &dd::New<new_type>>::Get();
		typeInfo->PlacementNew = SetFunc<HasDefaultCtor<T>::value, void (*)(void*), &dd::PlacementNew<new_type>>::Get();
		typeInfo->Copy = SetFunc<HasCopyCtor<T>::value, void(*)(void*, const void*), &dd::Copy<new_type>>::Get();
		typeInfo->Delete = dd::Delete<T>;
		typeInfo->PlacementCopy = SetFunc<HasCopyCtor<T>::value, void(*)(void*, const void*), &dd::PlacementCopy<new_type>>::Get();
		typeInfo->PlacementDelete = dd::PlacementDelete<T>;
		typeInfo->NewCopy = SetFunc<HasCopyCtor<T>::value, void(*)(void**, const void*), &dd::NewCopy<new_type>>::Get();
		typeInfo->SerializeCustom = nullptr;
		typeInfo->DeserializeCustom = nullptr;

		sm_typeMap.Add( name, typeInfo );

		T::RegisterMembers();

		return typeInfo;
	}

	template <typename T>
	const TypeInfo* TypeInfo::RegisterPOD( const char* name )
	{
		TypeInfo* typeInfo = const_cast<TypeInfo*>( GetType<T>() );
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
		TypeInfo* typeInfo = const_cast<TypeInfo*>( GetType<T>() );
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
		typeInfo->PlacementNew = SetFunc<HasDefaultCtor<T>::value, void (*)(void*), &dd::PlacementNew<new_type>>::Get();
		typeInfo->Copy = SetFunc<HasCopyCtor<T>::value, void(*)(void*, const void*), &dd::Copy<new_type>>::Get();
		typeInfo->Delete = dd::Delete<T>;
		typeInfo->PlacementCopy = SetFunc<HasCopyCtor<T>::value, void(*)(void*, const void*), &dd::PlacementCopy<new_type>>::Get();
		typeInfo->PlacementDelete = dd::PlacementDelete<T>;
		typeInfo->NewCopy = SetFunc<HasCopyCtor<T>::value, void(*)(void**, const void*), &dd::NewCopy<new_type>>::Get();

		typeInfo->SerializeCustom = dd::Serialize::SerializeContainer<T>;
		typeInfo->DeserializeCustom = dd::Serialize::DeserializeContainer<T>;

		sm_typeMap.Add( finalName, typeInfo );

		return typeInfo;
	}

	template <typename FnType>
	void TypeInfo::AddMethod( Function f, FnType fn, const char* name )
	{
		Method& m = m_methods.Allocate();
		m.Name = name;
		m.Function = f;

		const FunctionSignature* sig = f.Signature();
		String128 signature;

		if( sig->GetRet() != nullptr )
			signature += sig->GetRet()->Name();
		else
			signature += "void";

		signature += " ";
		signature += name;

		signature += "(";

		uint argCount = sig->ArgCount();
		for( uint i = 0; i < argCount; ++i )
		{
			signature += sig->GetArg( i )->Name();

			if( i < (argCount - 1) )
				signature += ",";
		}

		signature += ")";
		
		//ScriptEngine::GetInstance()->RegisterMethod( sig->GetContext()->Name(), signature, fn );
	}
};
