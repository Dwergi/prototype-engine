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
Creation Date : Sat Jan 11 14:03:56 2014
File Name     : SELSignature.h
Purpose       : 
*/

#pragma once

#include "TypeInfoMacros.h"

namespace dd
{
	class TypeInfo;

	class FunctionSignature
	{
	private:
		const TypeInfo* m_ret;
		const TypeInfo** m_args;
		const TypeInfo* m_context;
		uint m_argCount;

	public:
		uint ArgCount() const;
		const TypeInfo* GetRet() const;
		const TypeInfo* GetArg( uint i ) const;
		const TypeInfo* GetContext() const;

		FunctionSignature();
		FunctionSignature( const FunctionSignature& rhs );
		FunctionSignature& operator=( const FunctionSignature& rhs );

		// Static function with return value
		template <typename R>
		FunctionSignature( R (*fn)() )
			: m_ret( GET_TYPE( R ) )
			, m_args( nullptr )
			, m_argCount( 0 )
			, m_context( nullptr )
		{
		}

		// Helper to use list initialization to evaluate side-effects with variadic templates.
		// See: http://stackoverflow.com/questions/17339789/how-to-call-a-function-on-all-variadic-template-args
		struct ExpandType
		{
			template< typename... Ts >
			ExpandType( Ts&&... ) {}
		};

		template <typename... Args, std::size_t... Index>
		void CreateArgs( const TypeInfo** argStack, std::index_sequence<Index...> ) const
		{
			ExpandType
			{
				0, (argStack[Index] = dd::TypeInfo::GetType<std::tuple_element_t<Index, std::tuple<Args...>>>(), 0)...
			};
		}

		template <typename R, typename... Args>
		FunctionSignature( R (*fn)( Args... ) )
			: m_ret( GET_TYPE( R ) )
			, m_context( nullptr )
		{
			constexpr const std::size_t ArgCount = sizeof...(Args);
			static const TypeInfo* s_args[ArgCount];

			CreateArgs<Args...>( s_args, std::make_index_sequence<ArgCount>() );

			m_args = s_args;
			m_argCount = ArgCount;
		}

		// Static function with void return
		FunctionSignature( void (*)() );

		template <typename... Args>
		FunctionSignature( void (*fn)( Args... ) )
			: m_ret( nullptr )
			, m_context( nullptr )
		{
			constexpr const std::size_t ArgCount = sizeof...(Args);
			static const TypeInfo* s_args[ArgCount];

			CreateArgs<Args...>( s_args, std::make_index_sequence<ArgCount>() );

			m_args = s_args;
			m_argCount = ArgCount;
		}

		// Method with return value, non-const
		template <typename C, typename R>
		FunctionSignature( R (C::*fn)() )
			: m_ret( GET_TYPE( R ) )
			, m_argCount( 0 )
			, m_args( nullptr )
			, m_context( GET_TYPE( C ) )
		{
		}

		template <typename C, typename R, typename... Args>
		FunctionSignature( R (C::*fn)( Args... ) )
			: m_ret( GET_TYPE( R ) )
			, m_context( GET_TYPE( C ) )
		{
			constexpr const std::size_t ArgCount = sizeof...(Args);
			static const TypeInfo* s_args[ArgCount];

			CreateArgs<Args...>( s_args, std::make_index_sequence<ArgCount>() );

			m_args = s_args;
			m_argCount = ArgCount;
		}

		
		// Method without return value, non-const
		template <typename C>
		FunctionSignature( void (C::*fn)() )
			: m_ret( nullptr )
			, m_argCount( 0 )
			, m_args( nullptr )
			, m_context( GET_TYPE( C ) )
		{
		}

		template <typename C, typename... Args>
		FunctionSignature( void (C::*fn)( Args... ) )
			: m_ret( nullptr )
			, m_context( GET_TYPE( C ) )
		{
			constexpr const std::size_t ArgCount = sizeof...(Args);
			static const TypeInfo* s_args[ArgCount];

			CreateArgs<Args...>( s_args, std::make_index_sequence<ArgCount>() );

			m_args = s_args;
			m_argCount = ArgCount;
		}

		// Method with return value, const
		template <typename C, typename R>
		FunctionSignature( R (C::*fn)() const )
			: m_ret( GET_TYPE( R ) )
			, m_argCount( 0 )
			, m_args( nullptr )
			, m_context( GET_TYPE( C ) )
		{
		}

		template <typename C, typename R, typename... Args>
		FunctionSignature( R (C::*fn)( Args... ) const )
			: m_ret( GET_TYPE( R ) )
			, m_context( GET_TYPE( C ) )
		{
			constexpr const std::size_t ArgCount = sizeof...(Args);
			static const TypeInfo* s_args[ArgCount];

			CreateArgs<Args...>( s_args, std::make_index_sequence<ArgCount>() );

			m_args = s_args;
			m_argCount = ArgCount;
		}
		
		// Method without return value, const
		template <typename C>
		FunctionSignature( void (C::*fn)() const )
			: m_ret( nullptr )
			, m_argCount( 0 )
			, m_args( nullptr )
			, m_context( GET_TYPE( C ) )
		{
		}

		template <typename C, typename... Args>
		FunctionSignature( void (C::*fn)( Args... ) const )
			: m_ret( nullptr )
			, m_context( GET_TYPE( C ) )
		{
			constexpr const std::size_t ArgCount = sizeof...(Args);
			static const TypeInfo* s_args[ArgCount];

			CreateArgs<Args...>( s_args, std::make_index_sequence<ArgCount>() );

			m_args = s_args;
			m_argCount = ArgCount;
		}
	};
}
