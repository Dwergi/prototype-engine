//
// WrenForeignFunction.h - Wrapper around a C++ function callable from Wren.
// Beware all ye who enter here. Variadic template magic abound.
// Copyright (C) Sebastian Nordgren 
// March 31st 2015
//

#pragma once

namespace dd
{
	namespace WrenInternal
	{
		// free functions
		template <typename R, typename... Args, std::size_t... Index>
		decltype(auto) invokeFromWrenHelper( WrenVM* vm, R (*fn)(Args...), std::index_sequence<Index...> )
		{
			return fn( WrenArgument<std::tuple_element_t<Index, std::tuple<Args...>>>::get( vm, Index + 1 )... );
		}

		template <typename R, typename... Args>
		decltype(auto) invokeFromWren( WrenVM* vm, R (*fn)(Args...) )
		{
			constexpr std::size_t ArgCount = sizeof...(Args);
			return invokeFromWrenHelper( vm, fn, std::make_index_sequence<ArgCount>() );
		}

		// methods
		template <typename R, typename C, typename... Args, std::size_t... Index>
		decltype(auto) invokeFromWrenHelper( WrenVM* vm, R (C::*fn)(Args...), std::index_sequence<Index...> )
		{
			C* obj = static_cast<C*>(wrenGetSlotForeign( vm, 0 ));
			return (obj->*fn)( WrenArgument<std::tuple_element_t<Index, std::tuple<Args...>>>::get( vm, Index + 1 )... );
		}

		template <typename R, typename C, typename... Args>
		decltype(auto) invokeFromWren( WrenVM* vm, R (C::*fn)(Args...) )
		{
			constexpr std::size_t ArgCount = sizeof...(Args);
			return invokeFromWrenHelper( vm, fn, std::make_index_sequence<ArgCount>() );
		}

		// const methods
		template <typename R, typename C, typename... Args, std::size_t... Index>
		decltype(auto) invokeFromWrenHelper( WrenVM* vm, R (C::*fn)(Args...) const, std::index_sequence<Index...> )
		{
			const C* obj = static_cast<const C*>(wrenGetSlotForeign( vm, 0 ));
			return (obj->*fn)( WrenArgument<std::tuple_element_t<Index, std::tuple<Args...>>>::get( vm, Index + 1 )... );
		}

		template <typename R, typename C, typename... Args>
		decltype(auto) invokeFromWren( WrenVM* vm, R (C::*fn)(Args...) const )
		{
			constexpr std::size_t ArgCount = sizeof...(Args);
			return invokeFromWrenHelper( vm, fn, std::make_index_sequence<ArgCount>() );
		}

		template <bool Return>
		struct WrenInvokeWithoutReturningIf
		{
			// free function
			template <typename R, typename... Args>
			static void invoke( WrenVM* vm, R (*fn)(Args...) )
			{
				WrenArgument<R>::set( vm, 0, invokeFromWren( vm, fn ) );
			}

			// method
			template <typename R, typename C, typename... Args>
			static void invoke( WrenVM* vm, R (C::*fn)(Args...) )
			{
				WrenArgument<R>::set( vm, 0, invokeFromWren( vm, fn ) );
			}

			// const method
			template <typename R, typename C, typename... Args>
			static void invoke( WrenVM* vm, R (C::*fn)(Args...) const )
			{
				WrenArgument<R>::set( vm, 0, invokeFromWren( vm, fn ) );
			}
		};

		// void case
		template <>
		struct WrenInvokeWithoutReturningIf<true>
		{
			// free function
			template <typename R, typename... Args>
			static void invoke( WrenVM* vm, R (*fn)(Args...) )
			{
				invokeFromWren( vm, fn );
			}

			template <typename R, typename C, typename... Args>
			static void invoke( WrenVM* vm, R (C::*fn)(Args...) )
			{
				invokeFromWren( vm, fn );
			}

			template <typename R, typename C, typename... Args>
			static void invoke( WrenVM* vm, R (C::*fn)(Args...) const )
			{
				invokeFromWren( vm, fn );
			}
		};

		template <typename FnType, FnType>
		struct WrenForeignFunction;

		// free function
		template <typename R, typename... Args, R (*fn)(Args...) >
		struct WrenForeignFunction<R( *)(Args...), fn>
		{
			static void call( WrenVM* vm )
			{
				WrenInvokeWithoutReturningIf<std::is_void<R>::value>::invoke( vm, fn );
			}
		};

		// method
		template <typename R, typename C, typename... Args, R (C::*fn)(Args...)>
		struct WrenForeignFunction< R (C::*)(Args...), fn>
		{
			static void call( WrenVM* vm )
			{
				WrenInvokeWithoutReturningIf<std::is_void<R>::value>::invoke( vm, fn );
			}
		};

		// const method
		template <typename R, typename C, typename... Args, R (C::*fn)(Args...) const>
		struct WrenForeignFunction< R (C::*)(Args...) const, fn>
		{
			static void call( WrenVM* vm )
			{
				WrenInvokeWithoutReturningIf<std::is_void<R>::value>::invoke( vm, fn );
			}
		};
	}
}