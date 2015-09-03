/*
Copyright (c) 2013 Randy Gaul http://RandyGaul.net

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Creator(s)    : Randy Gaul
Creation Date : Sat Jan 11 14:03:32 2014
File Name     : SELFunctionPtr.h
Purpose       : 
*/

#pragma once

namespace dd
{
	struct GenericBase1 {};
	struct GenericBase2 {};
	struct GenericMultiVirtualClass : public virtual GenericBase1, public GenericBase2 {};
	struct GenericMultiClass : public GenericBase1, public GenericBase2 {};
	struct GenericClass {};
	struct GenericUnknownClass;

	typedef void (GenericUnknownClass::* UnknownClassFn)  ();
	typedef void (GenericMultiClass::* MultiClassFn)      ();
	typedef void (GenericMultiVirtualClass::* MultiVirtFn)();
	typedef void (GenericClass::* GenericClassFn)         ();
	typedef void (* StaticFn)                             ();

	struct FnPtr
	{
		UnknownClassFn UnknownClassFn;
		MultiClassFn   MultiClassFn;
		MultiVirtFn    MultiVirtFn;
		GenericClassFn GenericClassFn;
		StaticFn       StaticFn;
	};

	// Static functions without return value
	inline void CallPtr( void (*ptr)(), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 0 );

		ptr();
	}

	template <typename Arg1>
	inline void CallPtr( void (*ptr)( Arg1 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 1 );

		ptr(
			args[0].GetValue<Arg1>()
			);
	}

	template <typename Arg1, typename Arg2>
	inline void CallPtr( void (*ptr)( Arg1, Arg2 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 2 );

		ptr(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>()
			);
	}

	template <typename Arg1, typename Arg2, typename Arg3>
	inline void CallPtr( void (*ptr)( Arg1, Arg2, Arg3 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 3 );

		ptr(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[1].GetValue<Arg3>()
			);
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	inline void CallPtr( void (*ptr)( Arg1, Arg2, Arg3, Arg4 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 4 );

		ptr(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[1].GetValue<Arg3>(),
			args[1].GetValue<Arg4>()
			);
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	inline void CallPtr( void (*ptr)( Arg1, Arg2, Arg3, Arg4, Arg5 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 5 );

		ptr(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[1].GetValue<Arg3>(),
			args[1].GetValue<Arg4>(),
			args[1].GetValue<Arg5>()
			);
	}

	// Static functions with return value
	template <typename R>
	inline void CallPtr( R (*ptr)(), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 0 );

		ret->GetValue<R>() = ptr();
	}

	template <typename R, typename Arg1>
	inline void CallPtr( R (*ptr)( Arg1 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 1 );

		ret->GetValue<R>() = ptr(
			args[0].GetValue<Arg1>()
			);
	}

	template <typename R, typename Arg1, typename Arg2>
	inline void CallPtr( R (*ptr)( Arg1, Arg2 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 2 );

		ret->GetValue<R>() = ptr(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>()
			);
	}

	template <typename R, typename Arg1, typename Arg2, typename Arg3>
	inline void CallPtr( R (*ptr)( Arg1, Arg2, Arg3 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 3 );

		ret->GetValue<R>() = ptr(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[1].GetValue<Arg3>()
			);
	}

	template <typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	inline void CallPtr( R (*ptr)( Arg1, Arg2, Arg3, Arg4 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 4 );

		ret->GetValue<R>() = ptr(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[1].GetValue<Arg3>(),
			args[1].GetValue<Arg4>()
			);
	}

	template <typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	inline void CallPtr( R (*ptr)( Arg1, Arg2, Arg3, Arg4, Arg5 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 5 );

		ret->GetValue<R>() = ptr(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[1].GetValue<Arg3>(),
			args[1].GetValue<Arg4>(),
			args[1].GetValue<Arg5>()
			);
	}

	// non const void return method
	template <typename C>
	void CallPtr( void (C::*ptr)(), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 0 );

		(((C *)context)->*ptr)();
	}

	template <typename C, typename Arg1>
	void CallPtr( void (C::*ptr)( Arg1 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 1 );

		(((C *)context)->*ptr)(
			args[0].GetValue<Arg1>()
			);
	}

	template <typename C, typename Arg1, typename Arg2>
	void CallPtr( void (C::*ptr)( Arg1, Arg2 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 2 );

		(((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>()
			);
	}

	template <typename C, typename Arg1, typename Arg2, typename Arg3>
	void CallPtr( void (C::*ptr)( Arg1, Arg2, Arg3 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 3 );

		(((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>(),
			args[0].GetValue<Arg3>()
			);
	}

	template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void CallPtr( void (C::*ptr)( Arg1, Arg2, Arg3, Arg4 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 4 );

		(((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>(),
			args[0].GetValue<Arg3>(),
			args[0].GetValue<Arg4>()
			);
	}

	template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void CallPtr( void (C::*ptr)( Arg1, Arg2, Arg3, Arg4, Arg5 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 5 );

		(((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>(),
			args[0].GetValue<Arg3>(),
			args[0].GetValue<Arg4>(),
			args[0].GetValue<Arg5>()
			);
	}

	// non const non void return method
	template <typename C, typename R>
	void CallPtr( R (C::*ptr)(), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 0 );

		ret->GetValue<R>() = (((C *)context)->*ptr)();
	}

	template <typename C, typename R, typename Arg1>
	void CallPtr( R (C::*ptr)( Arg1 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 1 );

		ret->GetValue<R>() = (((C *)context)->*ptr)(
			args[0].GetValue<Arg1>()
			);
	}

	template <typename C, typename R, typename Arg1, typename Arg2>
	void CallPtr( R (C::*ptr)( Arg1, Arg2 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 2 );

		ret->GetValue<R>() = (((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>()
			);
	}

	template <typename C, typename R, typename Arg1, typename Arg2, typename Arg3>
	void CallPtr( R (C::*ptr)( Arg1, Arg2, Arg3 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 3 );

		ret->GetValue<R>() = (((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>(),
			args[0].GetValue<Arg3>()
			);
	}

	template <typename C, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void CallPtr( R (C::*ptr)( Arg1, Arg2, Arg3, Arg4 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 4 );

		ret->GetValue<R>() = (((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>(),
			args[0].GetValue<Arg3>(),
			args[0].GetValue<Arg4>()
			);
	}

	template <typename C, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void CallPtr( R (C::*ptr)( Arg1, Arg2, Arg3, Arg4, Arg5 ), Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 5 );

		(((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>(),
			args[0].GetValue<Arg3>(),
			args[0].GetValue<Arg4>(),
			args[0].GetValue<Arg5>()
			);
	}

	// const void return method
	template <typename C>
	void CallPtr( void (C::*ptr)() const, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 0 );

		(((C *)context)->*ptr)();
	}

	template <typename C, typename Arg1>
	void CallPtr( void (C::*ptr)( Arg1 ) const, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 1 );

		(((C *)context)->*ptr)(
			args[0].GetValue<Arg1>()
			);
	}

	template <typename C, typename Arg1, typename Arg2>
	void CallPtr( void (C::*ptr)( Arg1, Arg2 ) const, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 2 );

		(((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>()
			);
	}

	template <typename C, typename Arg1, typename Arg2, typename Arg3>
	void CallPtr( void (C::*ptr)( Arg1, Arg2, Arg3 ) const, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 3 );

		(((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>(),
			args[0].GetValue<Arg3>()
			);
	}

	template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void CallPtr( void (C::*ptr)( Arg1, Arg2, Arg3, Arg4 ) const, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 4 );

		(((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>(),
			args[0].GetValue<Arg3>(),
			args[0].GetValue<Arg4>()
			);
	}

	template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void CallPtr( void (C::*ptr)( Arg1, Arg2, Arg3, Arg4, Arg5 ) const, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 5 );

		(((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>(),
			args[0].GetValue<Arg3>(),
			args[0].GetValue<Arg4>(),
			args[0].GetValue<Arg5>()
			);
	}

	// const non void return method
	template <typename C, typename R>
	void CallPtr( R (C::*ptr)() const, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 0 );

		ret->GetValue<R>() = (((C *)context)->*ptr)();
	}

	template <typename C, typename R, typename Arg1>
	void CallPtr( R (C::*ptr)( Arg1 ) const, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 1 );

		ret->GetValue<R>() = (((C *)context)->*ptr)(
			args[0].GetValue<Arg1>()
			);
	}

	template <typename C, typename R, typename Arg1, typename Arg2>
	void CallPtr( R (C::*ptr)( Arg1, Arg2 ) const, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 2 );

		ret->GetValue<R>() = (((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>()
			);
	}

	template <typename C, typename R, typename Arg1, typename Arg2, typename Arg3>
	void CallPtr( R (C::*ptr)( Arg1, Arg2, Arg3 ) const, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 3 );

		ret->GetValue<R>() = (((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>(),
			args[0].GetValue<Arg3>()
			);
	}

	template <typename C, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void CallPtr( R (C::*ptr)( Arg1, Arg2, Arg3, Arg4 ) const, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 4 );

		ret->GetValue<R>() = (((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>(),
			args[0].GetValue<Arg3>(),
			args[0].GetValue<Arg4>()
			);
	}

	template <typename C, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void CallPtr( R (C::*ptr)( Arg1, Arg2, Arg3, Arg4, Arg5 ) const, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		ASSERT( argCount == 5 );

		(((C *)context)->*ptr)(
			args[0].GetValue<Arg1>(),
			args[0].GetValue<Arg2>(),
			args[0].GetValue<Arg3>(),
			args[0].GetValue<Arg4>(),
			args[0].GetValue<Arg5>()
			);
	}

	template <typename FunctionPtrType>
	void CallHelper( const FnPtr* ptr, Variable* ret, void* context, Variable* args, unsigned argCount )
	{
		CallPtr( *(FunctionPtrType *)ptr, ret, context, args, argCount );
	}

	// Lua function call binding
	template <>
	void CallHelper<const char* >( const FnPtr* ptr, Variable* ret, void* context, Variable* args, unsigned argCount );

	class FunctionPtr
	{
	public:
		template <typename FunctionPtrType>
		FunctionPtr( FunctionPtrType ptr )
			: m_sig( ptr )
			, m_ptr( *((FnPtr*) &ptr) )
			, m_call( CallHelper<FunctionPtrType> )
		{
		}

		FunctionPtr( const dd::String& functionName )
			: m_sig()
			, m_call( CallHelper<const char* > )
		{
			ASSERT( functionName.Length() < sizeof( FnPtr ) - 1, "Function name %s too long to bind.", functionName );

			char* dest = (char*) &m_ptr;
			memcpy( dest, functionName.c_str(), functionName.Length() );
			dest[ functionName.Length() ] = '\0'; // null-terminate
		}

		// Used to bind a context for calling methods
		template <typename Context>
		void Bind( Context& c )
		{
			m_context = &c;
		}

		template <typename Context>
		void Bind( Context *c )
		{
			m_context = c;
		}

		Variable& Context();
		const Variable& Context() const;
		bool IsMethod() const;

		// void return
		inline void operator()() const
		{
			m_call( &m_ptr, NULL, m_context.GetData(), NULL, 0 );
		}
		template <typename Arg1>
		void operator()( Arg1 arg1 ) const;
		template <typename Arg1, typename Arg2>
		void operator()( Arg1 arg1, Arg2 arg2 ) const;
		template <typename Arg1, typename Arg2, typename Arg3>
		void operator()( Arg1 arg1, Arg2 arg2, Arg3 arg3 ) const;
		template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		void operator()( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4 ) const;
		template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		void operator()( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5 ) const;

		// non void return
		inline void operator()( Variable& ret ) const
		{
			m_call( &m_ptr, &ret, m_context.GetData(), NULL, 0 );
		}
		template <typename Arg1>
		void operator()( Variable& ret, Arg1 arg1 ) const;
		template <typename Arg1, typename Arg2>
		void operator()( Variable& ret, Arg1 arg1, Arg2 arg2 ) const;
		template <typename Arg1, typename Arg2, typename Arg3>
		void operator()( Variable& ret, Arg1 arg1, Arg2 arg2, Arg3 arg3 ) const;
		template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		void operator()( Variable& ret, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4 ) const;
		template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		void operator()( Variable& ret, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5 ) const;

	private:
		FunctionSignature m_sig;
		FnPtr m_ptr;
		Variable m_context;

		void (*m_call)( const FnPtr* ptr, Variable* , void* , Variable* , unsigned );
	};

	// void return
	template <typename Arg1>
	void FunctionPtr::operator()( Arg1 arg1 ) const
	{
		ASSERT( m_sig.ArgCount() == 1 );
		ASSERT( m_sig.GetArg( 0 ) == SEL_TYPE( Arg1 ) );

		Variable argStack[1];

		new (argStack) Variable( arg1 );

		m_call( &m_ptr, NULL, m_context.GetData(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2>
	void FunctionPtr::operator()( Arg1 arg1, Arg2 arg2 ) const
	{
		ASSERT( m_sig.ArgCount() == 2 );
		ASSERT( m_sig.GetArg( 0 ) == SEL_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == SEL_TYPE( Arg2 ) );

		Variable argStack[2];

		new (argStack) Variable( arg1 );
		new (argStack) Variable( arg2 );

		m_call( &m_ptr, NULL, m_context.GetData(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2, typename Arg3>
	void FunctionPtr::operator()( Arg1 arg1, Arg2 arg2, Arg3 arg3 ) const
	{
		ASSERT( m_sig.ArgCount() == 3 );
		ASSERT( m_sig.GetArg( 0 ) == SEL_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == SEL_TYPE( Arg2 ) );
		ASSERT( m_sig.GetArg( 2 ) == SEL_TYPE( Arg3 ) );

		Variable argStack[3];

		new (argStack) Variable( arg1 );
		new (argStack) Variable( arg2 );
		new (argStack) Variable( arg3 );

		m_call( &m_ptr, NULL, m_context.GetData(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void FunctionPtr::operator()( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4 ) const
	{
		ASSERT( m_sig.ArgCount() == 4 );
		ASSERT( m_sig.GetArg( 0 ) == SEL_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == SEL_TYPE( Arg2 ) );
		ASSERT( m_sig.GetArg( 2 ) == SEL_TYPE( Arg3 ) );
		ASSERT( m_sig.GetArg( 3 ) == SEL_TYPE( Arg4 ) );

		Variable argStack[4];

		new (argStack) Variable( arg1 );
		new (argStack) Variable( arg2 );
		new (argStack) Variable( arg3 );
		new (argStack) Variable( arg4 );

		m_call( &m_ptr, NULL, m_context.GetData(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void FunctionPtr::operator()( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5 ) const
	{
		ASSERT( m_sig.ArgCount() == 5 );
		ASSERT( m_sig.GetArg( 0 ) == SEL_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == SEL_TYPE( Arg2 ) );
		ASSERT( m_sig.GetArg( 2 ) == SEL_TYPE( Arg3 ) );
		ASSERT( m_sig.GetArg( 3 ) == SEL_TYPE( Arg4 ) );
		ASSERT( m_sig.GetArg( 4 ) == SEL_TYPE( Arg5 ) );

		Variable argStack[5];

		new (argStack) Variable( arg1 );
		new (argStack) Variable( arg2 );
		new (argStack) Variable( arg3 );
		new (argStack) Variable( arg4 );
		new (argStack) Variable( arg5 );

		m_call( &m_ptr, NULL, m_context.GetData(), argStack, m_sig.ArgCount() );
	}

	// non void return
	template <typename Arg1>
	void FunctionPtr::operator()( Variable& ret, Arg1 arg1 ) const
	{
		ASSERT( m_sig.ArgCount() == 1 );
		ASSERT( m_sig.GetArg( 0 ) == SEL_TYPE( Arg1 ) );

		Variable argStack[1];

		new (argStack) Variable( arg1 );

		m_call( &m_ptr, &ret, m_context.GetData(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2>
	void FunctionPtr::operator()( Variable& ret, Arg1 arg1, Arg2 arg2 ) const
	{
		ASSERT( m_sig.ArgCount() == 2 );
		ASSERT( m_sig.GetArg( 0 ) == SEL_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == SEL_TYPE( Arg2 ) );

		Variable argStack[2];

		new (argStack) Variable( arg1 );
		new (argStack) Variable( arg2 );

		m_call( &m_ptr, &ret, m_context.GetData(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2, typename Arg3>
	void FunctionPtr::operator()( Variable& ret, Arg1 arg1, Arg2 arg2, Arg3 arg3 ) const
	{
		ASSERT( m_sig.ArgCount() == 3 );
		ASSERT( m_sig.GetArg( 0 ) == SEL_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == SEL_TYPE( Arg2 ) );
		ASSERT( m_sig.GetArg( 2 ) == SEL_TYPE( Arg3 ) );

		Variable argStack[3];

		new (argStack) Variable( arg1 );
		new (argStack) Variable( arg2 );
		new (argStack) Variable( arg3 );

		m_call( &m_ptr, &ret, m_context.GetData(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void FunctionPtr::operator()( Variable& ret, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4 ) const
	{
		ASSERT( m_sig.ArgCount() == 4 );
		ASSERT( m_sig.GetArg( 0 ) == SEL_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == SEL_TYPE( Arg2 ) );
		ASSERT( m_sig.GetArg( 2 ) == SEL_TYPE( Arg3 ) );
		ASSERT( m_sig.GetArg( 3 ) == SEL_TYPE( Arg4 ) );

		Variable argStack[4];

		new (argStack) Variable( arg1 );
		new (argStack) Variable( arg2 );
		new (argStack) Variable( arg3 );
		new (argStack) Variable( arg4 );

		m_call( &m_ptr, &ret, m_context.GetData(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void FunctionPtr::operator()( Variable& ret, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5 ) const
	{
		ASSERT( m_sig.ArgCount() == 5 );
		ASSERT( m_sig.GetArg( 0 ) == SEL_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == SEL_TYPE( Arg2 ) );
		ASSERT( m_sig.GetArg( 2 ) == SEL_TYPE( Arg3 ) );
		ASSERT( m_sig.GetArg( 3 ) == SEL_TYPE( Arg4 ) );
		ASSERT( m_sig.GetArg( 4 ) == SEL_TYPE( Arg5 ) );

		Variable argStack[5];

		new (argStack) Variable( arg1 );
		new (argStack) Variable( arg2 );
		new (argStack) Variable( arg3 );
		new (argStack) Variable( arg4 );
		new (argStack) Variable( arg5 );

		m_call( &m_ptr, &ret, m_context.GetData(), argStack, m_sig.ArgCount() );
	}
}
