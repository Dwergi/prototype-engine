//
// FunctionSignature.h - A way to extract TypeInfo objects from function pointers.
// Copyright (C) Sebastian Nordgren
// Original version by Randy Gaul (RandyGaul.net)
// March 23rd 2016
//

#pragma once


namespace dd
{
	// Helper to use list initialization to evaluate side-effects with variadic templates.
	// See: http://stackoverflow.com/questions/17339789/how-to-call-a-function-on-all-variadic-template-args
	using ExpandType = int[];

	// Get the type-safe argument at the given index in the given args array.
	template <std::size_t Index, typename... Args>
	struct GetArg
	{
		using Type = std::tuple_element_t<Index, std::tuple<Args...>>;

		static Type get( Variable* args )
		{
			return args[Index].GetValue<Type>();
		}
	};

	// Static functions with return value
	// Specialized version for no arguments.
	template <typename FunctionType, FunctionType FunctionPtr, typename R>
	void Call( Variable* ret, void* context, Variable* args )
	{
		ret->GetValue<R>() = (*FunctionPtr)();
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename... Args, std::size_t... Index>
	void InternalCallHelper( Variable* ret, Variable* args, std::index_sequence<Index...> )
	{
		ret->GetValue<R>() = (*FunctionPtr)(GetArg<Index, Args...>::get( args )...);
	}

	// General case of 1-N arguments.
	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename First, typename... Args>
	void Call( Variable* ret, void* context, Variable* args )
	{
		constexpr const std::size_t ArgCount = sizeof...(Args) + 1; // +1 because of First

		InternalCallHelper<FunctionType, FunctionPtr, R, First, Args...>( ret, args, std::make_index_sequence<ArgCount>() );
	}

	// Call static function without return value
	// Specialized version for no arguments.
	template <typename FunctionType, FunctionType FunctionPtr>
	void CallVoid( Variable* ret, void* context, Variable* args )
	{
		(*FunctionPtr)();
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename... Args, std::size_t... Index>
	void InternalCallVoidHelper( Variable* args, std::index_sequence<Index...> )
	{
		(*FunctionPtr)(GetArg<Index, Args...>::get( args )...);
	}

	// General case of 1-N arguments.
	template <typename FunctionType, FunctionType FunctionPtr, typename First, typename... Args>
	void CallVoid( Variable* ret, void* context, Variable* args )
	{
		constexpr const std::size_t ArgCount = sizeof...(Args) + 1; // +1 because of First

		InternalCallVoidHelper<FunctionType, FunctionPtr, First, Args...>( args, std::make_index_sequence<ArgCount>() );
	}
	
	// Methods with return value
	// Specialized for no arguments.
	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C>
	void CallMethod( Variable* ret, void* context, Variable* args )
	{
		ret->GetValue<R>() = (((C*) context)->*FunctionPtr)();
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename... Args, std::size_t... Index>
	void InternalCallMethodHelper( Variable* ret, C* context, Variable* args, std::index_sequence<Index...> )
	{
		ret->GetValue<R>() = (context->*FunctionPtr)( GetArg<Index, Args...>::get( args )... );
	}

	// General case of 1-N arguments.
	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename First, typename... Args>
	void CallMethod( Variable* ret, void* context, Variable* args )
	{
		constexpr const std::size_t ArgCount = sizeof...(Args) + 1; // +1 because of First

		InternalCallMethodHelper<FunctionType, FunctionPtr, R, C, First, Args...>( ret, (C*) context, args, std::make_index_sequence<ArgCount>() );
	}

	// Methods without return value
	// Specialized for no arguments.
	template <typename FunctionType, FunctionType FunctionPtr, typename C>
	void CallMethodVoid( Variable* ret, void* context, Variable* args )
	{
		(((C*) context)->*FunctionPtr)();
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename... Args, std::size_t... Index>
	void InternalCallMethodVoidHelper( C* context, Variable* args, std::index_sequence<Index...> )
	{
		(context->*FunctionPtr)( GetArg<Index, Args...>::get( args )... );
	}

	// General case of 1-N arguments.
	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename First, typename... Args>
	void CallMethodVoid( Variable* ret, void* context, Variable* args )
	{
		constexpr const std::size_t ArgCount = sizeof...(Args) + 1; // +1 because of First

		InternalCallMethodVoidHelper<FunctionType, FunctionPtr, C, First, Args...>( (C*) context, args, std::make_index_sequence<ArgCount>() );
	}

	class Function
	{
		typedef void (*HelperType)( Variable*, void*, Variable* );

	public:
		const FunctionSignature* Signature() const;

		Function();
		Function( const Function& rhs );
		Function( Function&& rhs );
		Function& operator=( const Function& rhs );

		Variable& Context();
		const Variable& Context() const;
		bool IsMethod() const;

		template <typename Context>
		void Bind( Context& context )
		{
			m_context = Variable( context );
		}

		void Bind( Variable& context );

		// Static functions without return values
		template <typename... Args>
		Function( void( *fn )(Args...), HelperType helper );

		// Static functions with return values
		template <typename R, typename... Args>
		Function( R (*fn)(Args...), HelperType helper );

		// Methods without return values, non-const
		template <typename C, typename... Args>
		Function( void (C::*fn)( Args... ), HelperType helper );

		// Methods with return values, non-const
		template <typename R, typename C, typename... Args>
		Function( R (C::*fn)( Args... ), HelperType helper );

		// Methods without return values, const
		template <typename C, typename... Args>
		Function( void (C::*fn)( Args... ) const, HelperType helper );

		// Methods with return values, const
		template <typename R, typename C, typename... Args>
		Function( R (C::*fn)( Args... ) const, HelperType helper );

		void operator()( Variable& ret, Variable* args, uint argCount ) const;

		void operator()() const;
		void operator()( Variable& ret ) const;

		template <typename... Args>
		void operator()( Variable& ret, Args... args ) const;

		template <typename... Args>
		void operator()( Args... args ) const;

	private:
		Variable m_context;
		FunctionSignature m_sig;
		HelperType m_callHelper;

		template <std::size_t Index, typename... Args>
		void AssertType() const;

		template <typename... Args, std::size_t... Index>
		void CreateVariables( Variable* arg_stack, const std::tuple<Args...>& tuple, std::index_sequence<Index...> ) const;
	};

	// Static functions with return value
	template <typename R, typename... Args>
	Function::Function( R (*fn)(Args...), HelperType helper )
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}
	
	// Static functions without return value
	template <typename... Args>
	Function::Function( void (*fn)( Args... ), HelperType helper )
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	// Methods with return value, non-const
	template <typename R, typename C, typename... Args>
	Function::Function( R (C::*fn)( Args... ), HelperType helper )
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	// Methods without return value, non-const
	template <typename C, typename... Args>
	Function::Function( void (C::*fn)( Args... ), HelperType helper	)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	// Methods with return value, const
	template <typename R, typename C, typename... Args>
	Function::Function( R (C::*fn)( Args... ) const, HelperType helper )
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	// Methods without return value, const
	template <typename C, typename... Args>
	Function::Function(	void (C::*fn)( Args... ) const,	HelperType helper )
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	// Build static functions with return value
	template <typename FunctionType, FunctionType FunctionPtr, typename R>
	Function BuildFunction( R (*fn)() )
	{
		auto helper = &Call<FunctionType, FunctionPtr, R>;
		return Function( fn, helper );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename First, typename... Args>
	Function BuildFunction( R (*fn)( First, Args... ) )
	{
		auto helper = &Call<FunctionType, FunctionPtr, R, First, Args...>;
		return Function( fn, helper );
	}

	// Build static functions without return value
	template <typename FunctionType, FunctionType FunctionPtr>
	Function BuildFunction( void (*fn)() )
	{
		auto helper = &CallVoid<FunctionType, FunctionPtr>;
		return Function( fn, helper );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename First, typename... Args>
	Function BuildFunction( void (*fn)( First, Args... ) )
	{
		auto helper = &CallVoid<FunctionType, FunctionPtr, First, Args...>;
		return Function( fn, helper );
	}

	// Build methods with return value, non-const
	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C>
	Function BuildFunction( R (C::*fn)() )
	{
		auto helper = &CallMethod<FunctionType, FunctionPtr, R, C>;
		return Function( fn, helper );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename First, typename... Args>
	Function BuildFunction( R (C::*fn)( First, Args... ) )
	{
		auto helper = &CallMethod<FunctionType, FunctionPtr, R, C, First, Args...>;
		return Function( fn, helper );
	}

	// Build methods with return value, const
	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C>
	Function BuildFunction( R (C::*fn)() const )
	{
		auto helper = &CallMethod<FunctionType, FunctionPtr, R, C>;
		return Function( fn, helper );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename First, typename... Args>
	Function BuildFunction( R (C::*fn)( First, Args... ) const )
	{
		auto helper = &CallMethod<FunctionType, FunctionPtr, R, C, First, Args...>;
		return Function( fn, helper );
	}

	// Build methods without return value, non-const
	template <typename FunctionType, FunctionType FunctionPtr, typename C>
	Function BuildFunction( void (C::*fn)() )
	{
		auto helper = &CallMethodVoid<FunctionType, FunctionPtr, C>;
		return Function( fn, helper );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename First, typename... Args>
	Function BuildFunction( void (C::*fn)( First, Args... ) )
	{
		auto helper = &CallMethodVoid<FunctionType, FunctionPtr, C, First, Args...>;
		return Function( fn, helper );
	}

	// Build methods without return value, const
	template <typename FunctionType, FunctionType FunctionPtr, typename C>
	Function BuildFunction( void (C::*fn)() const )
	{
		auto helper = &CallMethodVoid<FunctionType, FunctionPtr, C>;
		return Function( fn, helper );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename First, typename... Args>
	Function BuildFunction( void (C::*fn)( First, Args... ) const )
	{
		auto helper = &CallMethodVoid<FunctionType, FunctionPtr, C, First, Args...>;
		return Function( fn, helper );
	}

	// Creates an assert that the type at Index in the pack Args is of the same type as the signature.
	template <std::size_t Index, typename... Args>
	void Function::AssertType() const
	{
		const TypeInfo* typeInfo = TypeInfo::GetType<std::tuple_element_t<Index, std::tuple<Args...>>>();

		DD_ASSERT( m_sig.GetArg( Index ) == typeInfo );
	}

	template <typename... Args, std::size_t... Index>
	void Function::CreateVariables( Variable* arg_stack, const std::tuple<Args...>& tuple, std::index_sequence<Index...> ) const
	{
		// Expand the side effects using template fuckery.
		ExpandType
		{
			0, (arg_stack[Index] = Variable( std::get<Index>( tuple ) ), AssertType<Index, Args...>(), 0)...
		};
	}

	// Call with return value
	template <typename... Args>
	void Function::operator()( Variable& ret, Args... args ) const
	{
		constexpr const std::size_t ArgCount = sizeof...(Args);

		DD_ASSERT( m_sig.ArgCount() == ArgCount );
		
		Variable arg_stack[ArgCount];
		std::tuple<Args...> tuple = std::make_tuple( args... );

		CreateVariables( arg_stack, tuple, std::make_index_sequence<ArgCount>() );

		m_callHelper( &ret, m_context.Data(), arg_stack );

		DD_ASSERT( ret.Type() == m_sig.GetRet() );
	}

	// Call without return value
	template <typename... Args>
	void Function::operator()( Args... args ) const
	{
		constexpr const std::size_t ArgCount = sizeof...(Args);

		DD_ASSERT( m_sig.ArgCount() == ArgCount );

		Variable arg_stack[ArgCount];
		std::tuple<Args...> tuple = std::make_tuple( args... );

		CreateVariables( arg_stack, tuple, std::make_index_sequence<ArgCount>() );

		m_callHelper( nullptr, m_context.Data(), arg_stack );
	}
}
