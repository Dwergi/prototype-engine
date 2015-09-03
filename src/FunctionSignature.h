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

namespace dd
{
	class TypeInfo;

	class FunctionSignature
	{
	public:
		unsigned ArgCount() const;
		const TypeInfo* GetRet() const;
		const TypeInfo* GetArg( unsigned i ) const;
		const TypeInfo* GetContext() const;

		FunctionSignature();
		FunctionSignature( const FunctionSignature& rhs );
		FunctionSignature& operator=( const FunctionSignature& rhs );

		// Static function with return value
		template <typename R>
		FunctionSignature( R (*fn)() )
			: m_ret( SEL_TYPE( R ) )
			, m_args( NULL )
			, m_argCount( 0 )
			, m_context( NULL )
		{
		}

		template <typename R, typename Arg1>
		FunctionSignature( R (*fn)( Arg1 ) )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 1 )
			, m_context( NULL )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 )
			};
			m_args = args;
		}

		template <typename R, typename Arg1, typename Arg2>
		FunctionSignature( R (*fn)( Arg1, Arg2 ) )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 2 )
			, m_context( NULL )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
			};
			m_args = args;
		}

		template <typename R, typename Arg1, typename Arg2, typename Arg3>
		FunctionSignature( R (*fn)( Arg1, Arg2, Arg3 ) )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 3 )
			, m_context( NULL )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
			};
			m_args = args;
		}

		template <typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		FunctionSignature( R (*fn)( Arg1, Arg2, Arg3, Arg4 ) )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 4 )
			, m_context( NULL )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
				SEL_TYPE( Arg4 ),
			};
			m_args = args;
		}

		template <typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		FunctionSignature( R (*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ) )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 5 )
			, m_context( NULL )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
				SEL_TYPE( Arg4 ),
				SEL_TYPE( Arg5 ),
			};
			m_args = args;
		}

		// Static function with void return
		FunctionSignature( void (*)() );

		template <typename Arg1>
		FunctionSignature( void (*fn)( Arg1 ) )
			: m_ret( NULL )
			, m_argCount( 1 )
			, m_context( NULL )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
			};
			m_args = args;
		}

		template <typename Arg1, typename Arg2>
		FunctionSignature( void (*fn)( Arg1, Arg2 ) )
			: m_ret( NULL )
			, m_argCount( 2 )
			, m_context( NULL )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
			};
			m_args = args;
		}

		template <typename Arg1, typename Arg2, typename Arg3>
		FunctionSignature( void (*fn)( Arg1, Arg2, Arg3 ) )
			: m_ret( NULL )
			, m_argCount( 3 )
			, m_context( NULL )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
			};
			m_args = args;
		}

		template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		FunctionSignature( void (*fn)( Arg1, Arg2, Arg3, Arg4 ) )
			: m_ret( NULL )
			, m_argCount( 4 )
			, m_context( NULL )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
				SEL_TYPE( Arg4 ),
			};
			m_args = args;
		}

		template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		FunctionSignature( void (*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ) )
			: m_ret( NULL )
			, m_argCount( 5 )
			, m_context( NULL )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
				SEL_TYPE( Arg4 ),
				SEL_TYPE( Arg5 ),
			};
			m_args = args;
		}

		// Method with return value, non-const
		template <typename C, typename R>
		FunctionSignature( R (C::*)() )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 0 )
			, m_args( NULL )
			, m_context( SEL_TYPE( C ) )
		{
		}

		template <typename C, typename R, typename Arg1>
		FunctionSignature( R (C::*)( Arg1 ) )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 1 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
			};
			m_args = args;
		}

		template <typename C, typename R, typename Arg1, typename Arg2>
		FunctionSignature( R (C::*)( Arg1, Arg2 ) )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 2 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
			};
			m_args = args;
		}

		template <typename C, typename R, typename Arg1, typename Arg2, typename Arg3>
		FunctionSignature( R (C::*)( Arg1, Arg2, Arg3 ) )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 3 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
			};
			m_args = args;
		}

		template <typename C, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		FunctionSignature( R (C::*)( Arg1, Arg2, Arg3, Arg4 ) )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 4 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
				SEL_TYPE( Arg4 ),
			};
			m_args = args;
		}

		template <typename C, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		FunctionSignature( R (C::*)( Arg1, Arg2, Arg3, Arg4, Arg5 ) )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 5 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
				SEL_TYPE( Arg4 ),
				SEL_TYPE( Arg5 ),
			};
			m_args = args;
		}

		// Method without return value, non-const
		template <typename C>
		FunctionSignature( void (C::*)() )
			: m_ret( NULL )
			, m_argCount( 0 )
			, m_args( NULL )
			, m_context( SEL_TYPE( C ) )
		{
		}

		template <typename C, typename Arg1>
		FunctionSignature( void (C::*)( Arg1 ) )
			: m_ret( NULL )
			, m_argCount( 1 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
			};
			m_args = args;
		}

		template <typename C, typename Arg1, typename Arg2>
		FunctionSignature( void (C::*)( Arg1, Arg2 ) )
			: m_ret( NULL )
			, m_argCount( 2 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
			};
			m_args = args;
		}

		template <typename C, typename Arg1, typename Arg2, typename Arg3>
		FunctionSignature( void (C::*)( Arg1, Arg2, Arg3 ) )
			: m_ret( NULL )
			, m_argCount( 3 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
			};
			m_args = args;
		}

		template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		FunctionSignature( void (C::*)( Arg1, Arg2, Arg3, Arg4 ) )
			: m_ret( NULL )
			, m_argCount( 4 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
				SEL_TYPE( Arg4 ),
			};
			m_args = args;
		}

		template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		FunctionSignature( void (C::*)( Arg1, Arg2, Arg3, Arg4, Arg5 ) )
			: m_ret( NULL )
			, m_argCount( 5 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
				SEL_TYPE( Arg4 ),
				SEL_TYPE( Arg5 ),
			};
			m_args = args;
		}

		// Method with return value, const
		template <typename C, typename R>
		FunctionSignature( R (C::*)() const )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 0 )
			, m_args( NULL )
			, m_context( SEL_TYPE( C ) )
		{
		}

		template <typename C, typename R, typename Arg1>
		FunctionSignature( R (C::*)( Arg1 ) const )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 1 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
			};
			m_args = args;
		}

		template <typename C, typename R, typename Arg1, typename Arg2>
		FunctionSignature( R (C::*)( Arg1, Arg2 ) const )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 2 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
			};
			m_args = args;
		}

		template <typename C, typename R, typename Arg1, typename Arg2, typename Arg3>
		FunctionSignature( R (C::*)( Arg1, Arg2, Arg3 ) const )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 3 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
			};
			m_args = args;
		}

		template <typename C, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		FunctionSignature( R (C::*)( Arg1, Arg2, Arg3, Arg4 ) const )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 4 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
				SEL_TYPE( Arg4 ),
			};
			m_args = args;
		}

		template <typename C, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		FunctionSignature( R (C::*)( Arg1, Arg2, Arg3, Arg4, Arg5 ) const )
			: m_ret( SEL_TYPE( R ) )
			, m_argCount( 5 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
				SEL_TYPE( Arg4 ),
				SEL_TYPE( Arg5 ),
			};
			m_args = args;
		}

		// Method without return value, const
		template <typename C>
		FunctionSignature( void (C::*)() const )
			: m_ret( NULL )
			, m_argCount( 0 )
			, m_args( NULL )
			, m_context( SEL_TYPE( C ) )
		{
		}

		template <typename C, typename Arg1>
		FunctionSignature( void (C::*)( Arg1 ) const )
			: m_ret( NULL )
			, m_argCount( 1 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
			};
			m_args = args;
		}

		template <typename C, typename Arg1, typename Arg2>
		FunctionSignature( void (C::*)( Arg1, Arg2 ) const )
			: m_ret( NULL )
			, m_argCount( 2 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
			};
			m_args = args;
		}

		template <typename C, typename Arg1, typename Arg2, typename Arg3>
		FunctionSignature( void (C::*)( Arg1, Arg2, Arg3 ) const )
			: m_ret( NULL )
			, m_argCount( 3 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
			};
			m_args = args;
		}

		template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		FunctionSignature( void (C::*)( Arg1, Arg2, Arg3, Arg4 ) const )
			: m_ret( NULL )
			, m_argCount( 4 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
				SEL_TYPE( Arg4 ),
			};
			m_args = args;
		}

		template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		FunctionSignature( void (C::*)( Arg1, Arg2, Arg3, Arg4, Arg5 ) const )
			: m_ret( NULL )
			, m_argCount( 5 )
			, m_context( SEL_TYPE( C ) )
		{
			static const TypeInfo* args[] = {
				SEL_TYPE( Arg1 ),
				SEL_TYPE( Arg2 ),
				SEL_TYPE( Arg3 ),
				SEL_TYPE( Arg4 ),
				SEL_TYPE( Arg5 ),
			};
			m_args = args;
		}

	private:
		const TypeInfo* m_ret;
		const TypeInfo* *m_args;
		const TypeInfo* m_context;
		unsigned m_argCount;
	};
}
