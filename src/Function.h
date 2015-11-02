/*
Copyright (c) 2013-2014 Randy Gaul http://RandyGaul.net

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
Creation Date : Wed Nov 27 01:06:24 2013
File Name     : SELFunction.h
Purpose       : 
*/

#pragma once

namespace dd
{
	// Static functions with return value
	template <typename FunctionType, FunctionType FunctionPtr, typename R>
	void Call( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 0 );

		ret->GetValue<R>() = (*FunctionPtr)();
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename Arg1>
	void Call( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 1 );

		ret->GetValue<R>() = (*FunctionPtr)(
			args[0].GetValue<Arg1>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename Arg1, typename Arg2>
	void Call( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 2 );

		ret->GetValue<R>() = (*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename Arg1, typename Arg2, typename Arg3>
	void Call( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 3 );

		ret->GetValue<R>() = (*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[2].GetValue<Arg3>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void Call( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 4 );

		ret->GetValue<R>() = (*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[2].GetValue<Arg3>(),
			args[3].GetValue<Arg4>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void Call( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 5 );

		ret->GetValue<R>() = (*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[2].GetValue<Arg3>(),
			args[3].GetValue<Arg4>(),
			args[4].GetValue<Arg5>()
			);
	}

	// Call static function without return value
	template <typename FunctionType, FunctionType FunctionPtr>
	void CallVoid( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 0 );

		(*FunctionPtr)();
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename Arg1>
	void CallVoid( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 1 );

		(*FunctionPtr)(
			args[0].GetValue<Arg1>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename Arg1, typename Arg2>
	void CallVoid( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 2 );

		(*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename Arg1, typename Arg2, typename Arg3>
	void CallVoid( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 3 );

		(*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[2].GetValue<Arg3>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void CallVoid( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 4 );

		(*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[2].GetValue<Arg3>(),
			args[3].GetValue<Arg4>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void CallVoid( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 5 );

		(*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[2].GetValue<Arg3>(),
			args[3].GetValue<Arg4>(),
			args[4].GetValue<Arg5>()
			);
	}

	// Methods with return value
	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C>
	void CallMethod( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 0 );
		ret->GetValue<R>() = (((C*) context)->*FunctionPtr)();
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1>
	void CallMethod( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 1 );
		ret->GetValue<R>() = (((C*) context)->*FunctionPtr)(
			args[0].GetValue<Arg1>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1, typename Arg2>
	void CallMethod( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 2 );
		ret->GetValue<R>() = (((C*) context)->*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1, typename Arg2, typename Arg3>
	void CallMethod( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 3 );
		ret->GetValue<R>() = (((C*) context)->*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[2].GetValue<Arg3>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void CallMethod( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 4 );
		ret->GetValue<R>() = (((C*) context)->*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[2].GetValue<Arg3>(),
			args[3].GetValue<Arg4>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void CallMethod( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 5 );
		ret->GetValue<R>() = (((C*) context)->*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[2].GetValue<Arg3>(),
			args[3].GetValue<Arg4>(),
			args[4].GetValue<Arg5>()
			);
	}

	// Methods without return value
	template <typename FunctionType, FunctionType FunctionPtr, typename C>
	void CallMethodVoid( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 0 );
		(((C*) context)->*FunctionPtr)();
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1>
	void CallMethodVoid( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 1 );
		(((C*) context)->*FunctionPtr)(
			args[0].GetValue<Arg1>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1, typename Arg2>
	void CallMethodVoid( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 2 );
		(((C*) context)->*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1, typename Arg2, typename Arg3>
	void CallMethodVoid( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 3 );
		(((C*) context)->*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[2].GetValue<Arg3>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void CallMethodVoid( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 4 );
		(((C*) context)->*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[2].GetValue<Arg3>(),
			args[3].GetValue<Arg4>()
			);
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void CallMethodVoid( Variable* ret, void* context, Variable* args, uint argCount )
	{
		ASSERT( argCount == 5 );
		(((C*) context)->*FunctionPtr)(
			args[0].GetValue<Arg1>(),
			args[1].GetValue<Arg2>(),
			args[2].GetValue<Arg3>(),
			args[3].GetValue<Arg4>(),
			args[4].GetValue<Arg5>()
			);
	}

	struct FunctionArgs
	{
		Vector<Variable> Arguments;
		Variable Context;
	};

	class Function
	{
		typedef void (*HelperType)(Variable*, void*, Variable*, unsigned);

	public:
		const FunctionSignature* Signature() const;

		Function();
		Function( const Function& rhs );
		Function( Function&& rhs );
		Function& operator=( const Function& rhs );

		Variable& Context();
		const Variable& Context() const;
		bool IsMethod() const;

		template<typename Context>
		void Bind( Context& context )
		{
			m_context = context;
		}

		void Bind( Variable& context );

		// Static functions with return values
		template <typename R>
		Function(
			R (*fn)(),
			HelperType helper
			);
		template <typename R, typename Arg1>
		Function(
			R (*fn)( Arg1 ),
			HelperType helper
			);
		template <typename R, typename Arg1, typename Arg2>
		Function(
			R (*fn)( Arg1, Arg2 ),
			HelperType helper
			);
		template <typename R, typename Arg1, typename Arg2, typename Arg3>
		Function(
			R (*fn)( Arg1, Arg2, Arg3 ),
			HelperType helper
			);
		template <typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		Function(
			R (*fn)( Arg1, Arg2, Arg3, Arg4 ),
			HelperType helper
			);
		template <typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		Function(
			R (*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ),
			HelperType helper
			);

		// Static functions without return values
		Function(
			void (*fn)(),
			HelperType helper
			);
		template <typename Arg1>
		Function(
			void (*fn)( Arg1 ),
			HelperType helper
			);
		template <typename Arg1, typename Arg2>
		Function(
			void (*fn)( Arg1, Arg2 ),
			HelperType helper
			);
		template <typename Arg1, typename Arg2, typename Arg3>
		Function(
			void (*fn)( Arg1, Arg2, Arg3 ),
			HelperType helper
			);
		template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		Function(
			void (*fn)( Arg1, Arg2, Arg3, Arg4 ),
			HelperType helper
			);
		template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		Function(
			void (*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ),
			HelperType helper
			);

		// Methods with return values, non-const
		template <typename R, typename C>
		Function(
			R (C::*fn)(),
			HelperType helper
			);
		template <typename R, typename C, typename Arg1>
		Function(
			R (C::*fn)( Arg1 ),
			HelperType helper
			);
		template <typename R, typename C, typename Arg1, typename Arg2>
		Function(
			R (C::*fn)( Arg1, Arg2 ),
			HelperType helper
			);
		template <typename R, typename C, typename Arg1, typename Arg2, typename Arg3>
		Function(
			R (C::*fn)( Arg1, Arg2, Arg3 ),
			HelperType helper
			);
		template <typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		Function(
			R (C::*fn)( Arg1, Arg2, Arg3, Arg4 ),
			HelperType helper
			);
		template <typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		Function(
			R (C::*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ),
			HelperType helper
			);

		// Methods without return values, non-const
		template <typename C>
		Function(
			void (C::*fn)(),
			HelperType helper
			);
		template <typename C, typename Arg1>
		Function(
			void (C::*fn)( Arg1 ),
			HelperType helper
			);
		template <typename C, typename Arg1, typename Arg2>
		Function(
			void (C::*fn)( Arg1, Arg2 ),
			HelperType helper
			);
		template <typename C, typename Arg1, typename Arg2, typename Arg3>
		Function(
			void (C::*fn)( Arg1, Arg2, Arg3 ),
			HelperType helper
			);
		template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		Function(
			void (C::*fn)( Arg1, Arg2, Arg3, Arg4 ),
			HelperType helper
			);
		template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		Function(
			void (C::*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ),
			HelperType helper
			);

		// Methods with return values, const
		template <typename R, typename C>
		Function(
			R (C::*fn)() const,
			HelperType helper
			);
		template <typename R, typename C, typename Arg1>
		Function(
			R (C::*fn)( Arg1 ) const,
			HelperType helper
			);
		template <typename R, typename C, typename Arg1, typename Arg2>
		Function(
			R (C::*fn)( Arg1, Arg2 ) const,
			HelperType helper
			);
		template <typename R, typename C, typename Arg1, typename Arg2, typename Arg3>
		Function(
			R (C::*fn)( Arg1, Arg2, Arg3 ) const,
			HelperType helper
			);
		template <typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		Function(
			R (C::*fn)( Arg1, Arg2, Arg3, Arg4 ) const,
			HelperType helper
			);
		template <typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		Function(
			R (C::*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ) const,
			HelperType helper
			);

		// Methods without return values, const
		template <typename C>
		Function(
			void (C::*fn)() const,
			HelperType helper
			);
		template <typename C, typename Arg1>
		Function(
			void (C::*fn)( Arg1 ) const,
			HelperType helper
			);
		template <typename C, typename Arg1, typename Arg2>
		Function(
			void (C::*fn)( Arg1, Arg2 ) const,
			HelperType helper
			);
		template <typename C, typename Arg1, typename Arg2, typename Arg3>
		Function(
			void (C::*fn)( Arg1, Arg2, Arg3 ) const,
			HelperType helper
			);
		template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		Function(
			void (C::*fn)( Arg1, Arg2, Arg3, Arg4 ) const,
			HelperType helper
			);
		template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		Function(
			void (C::*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ) const,
			HelperType helper
			);

		void operator()( Variable& ret, Variable* args, uint argCount ) const;
		void operator()( Variable& ret, Variable* args, uint argCount );

		void operator()( Variable& ret ) const;
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

		void operator()() const;
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

	private:
		Variable m_context;
		FunctionSignature m_sig;
		void (*m_callHelper)( Variable*, void*, Variable*, uint );
	};

	// Static functions with return value
	template <typename R>
	Function::Function(
		R (*fn)(),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename Arg1>
	Function::Function(
		R (*fn)( Arg1 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename Arg1, typename Arg2>
	Function::Function(
		R (*fn)( Arg1, Arg2 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename Arg1, typename Arg2, typename Arg3>
	Function::Function(
		R (*fn)( Arg1, Arg2, Arg3 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Function::Function(
		R (*fn)( Arg1, Arg2, Arg3, Arg4 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Function::Function(
		R (*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	// Static functions without return value
	template <typename Arg1>
	Function::Function(
		void (*fn)( Arg1 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename Arg1, typename Arg2>
	Function::Function(
		void (*fn)( Arg1, Arg2 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename Arg1, typename Arg2, typename Arg3>
	Function::Function(
		void (*fn)( Arg1, Arg2, Arg3 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Function::Function(
		void (*fn)( Arg1, Arg2, Arg3, Arg4 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Function::Function(
		void (*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	// Methods with return value, non-const
	template <typename R, typename C>
	Function::Function(
		R (C::*fn)(),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename C, typename Arg1>
	Function::Function(
		R (C::*fn)( Arg1 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename C, typename Arg1, typename Arg2>
	Function::Function(
		R (C::*fn)( Arg1, Arg2 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename C, typename Arg1, typename Arg2, typename Arg3>
	Function::Function(
		R (C::*fn)( Arg1, Arg2, Arg3 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Function::Function(
		R (C::*fn)( Arg1, Arg2, Arg3, Arg4 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Function::Function(
		R (C::*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	// Methods without return value, non-const
	template <typename C>
	Function::Function(
		void (C::*fn)(),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename C, typename Arg1>
	Function::Function(
		void (C::*fn)( Arg1 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename C, typename Arg1, typename Arg2>
	Function::Function(
		void (C::*fn)( Arg1, Arg2 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename C, typename Arg1, typename Arg2, typename Arg3>
	Function::Function(
		void (C::*fn)( Arg1, Arg2, Arg3 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Function::Function(
		void (C::*fn)( Arg1, Arg2, Arg3, Arg4 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Function::Function(
		void (C::*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ),
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	// Methods with return value, const
	template <typename R, typename C>
	Function::Function(
		R (C::*fn)() const,
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename C, typename Arg1>
	Function::Function(
		R (C::*fn)( Arg1 ) const,
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename C, typename Arg1, typename Arg2>
	Function::Function(
		R (C::*fn)( Arg1, Arg2 ) const,
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename C, typename Arg1, typename Arg2, typename Arg3>
	Function::Function(
		R (C::*fn)( Arg1, Arg2, Arg3 ) const,
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Function::Function(
		R (C::*fn)( Arg1, Arg2, Arg3, Arg4 ) const,
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Function::Function(
		R (C::*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ) const,
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	// Methods without return value, const
	template <typename C>
	Function::Function(
		void (C::*fn)() const,
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename C, typename Arg1>
	Function::Function(
		void (C::*fn)( Arg1 ) const,
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename C, typename Arg1, typename Arg2>
	Function::Function(
		void (C::*fn)( Arg1, Arg2 ) const,
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename C, typename Arg1, typename Arg2, typename Arg3>
	Function::Function(
		void (C::*fn)( Arg1, Arg2, Arg3 ) const,
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Function::Function(
		void (C::*fn)( Arg1, Arg2, Arg3, Arg4 ) const,
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	template <typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Function::Function(
		void (C::*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ) const,
		HelperType helper
		)
		: m_sig( fn )
		, m_callHelper( helper )
	{
	}

	// Build static functions with return value
	template <typename FunctionType, FunctionType FunctionPtr, typename R>
	Function BuildFunction(R (*fn)() )
	{
		return Function(fn, &Call<FunctionType, FunctionPtr, R> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename Arg1>
	Function BuildFunction(R (*fn)( Arg1 ) )
	{
		return Function(fn, &Call<FunctionType, FunctionPtr, R, Arg1> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename Arg1, typename Arg2>
	Function BuildFunction(R (*fn)( Arg1, Arg2 ) )
	{
		return Function(fn, &Call<FunctionType, FunctionPtr, R, Arg1, Arg2> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename Arg1, typename Arg2, typename Arg3>
	Function BuildFunction(R (*fn)( Arg1, Arg2, Arg3 ) )
	{
		return Function(fn, &Call<FunctionType, FunctionPtr, R, Arg1, Arg2, Arg3> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Function BuildFunction(R (*fn)( Arg1, Arg2, Arg3, Arg4 ) )
	{
		return Function(fn, &Call<FunctionType, FunctionPtr, R, Arg1, Arg2, Arg3, Arg4> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Function BuildFunction(R (*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ) )
	{
		return Function(fn, &Call<FunctionType, FunctionPtr, R, Arg1, Arg2, Arg3, Arg4, Arg5> );
	}

	// Build static functions without return value
	template <typename FunctionType, FunctionType FunctionPtr>
	Function BuildFunction(void (*fn)() )
	{
		return Function(fn, &CallVoid<FunctionType, FunctionPtr> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename Arg1>
	Function BuildFunction(void (*fn)( Arg1 ) )
	{
		return Function(fn, &CallVoid<FunctionType, FunctionPtr, Arg1> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename Arg1, typename Arg2>
	Function BuildFunction(void (*fn)( Arg1, Arg2 ) )
	{
		return Function(fn, &CallVoid<FunctionType, FunctionPtr, Arg1, Arg2> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename Arg1, typename Arg2, typename Arg3>
	Function BuildFunction(void (*fn)( Arg1, Arg2, Arg3 ) )
	{
		return Function(fn, &CallVoid<FunctionType, FunctionPtr, Arg1, Arg2, Arg3> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Function BuildFunction(void (*fn)( Arg1, Arg2, Arg3, Arg4 ) )
	{
		return Function(fn, &CallVoid<FunctionType, FunctionPtr, Arg1, Arg2, Arg3, Arg4> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Function BuildFunction(void (*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ) )
	{
		return Function(fn, &CallVoid<FunctionType, FunctionPtr, Arg1, Arg2, Arg3, Arg4, Arg5> );
	}

	// Build methods with return value, non-const
	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C>
	Function BuildFunction(R (C::*fn)() )
	{
		return Function(fn, &CallMethod<FunctionType, FunctionPtr, R, C> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1>
	Function BuildFunction(R (C::*fn)( Arg1 ) )
	{
		return Function(fn, &CallMethod<FunctionType, FunctionPtr, R, C, Arg1> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1, typename Arg2>
	Function BuildFunction(R (C::*fn)( Arg1, Arg2 ) )
	{
		return Function(fn, &CallMethod<FunctionType, FunctionPtr, R, C, Arg1, Arg2> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1, typename Arg2, typename Arg3>
	Function BuildFunction(R (C::*fn)( Arg1, Arg2, Arg3 ) )
	{
		return Function(fn, &CallMethod<FunctionType, FunctionPtr, R, C, Arg1, Arg2, Arg3> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Function BuildFunction(R (C::*fn)( Arg1, Arg2, Arg3, Arg4 ) )
	{
		return Function(fn, &CallMethod<FunctionType, FunctionPtr, R, C, Arg1, Arg2, Arg3, Arg4> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Function BuildFunction(R (C::*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ) )
	{
		return Function(fn, &CallMethod<FunctionType, FunctionPtr, R, C, Arg1, Arg2, Arg3, Arg4, Arg5> );
	}

	// Build methods with return value, const
	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C>
	Function BuildFunction(R (C::*fn)() const )
	{
		return Function(fn, &CallMethod<FunctionType, FunctionPtr, R, C> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1>
	Function BuildFunction(R (C::*fn)( Arg1 ) const )
	{
		return Function(fn, &CallMethod<FunctionType, FunctionPtr, R, C, Arg1> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1, typename Arg2>
	Function BuildFunction(R (C::*fn)( Arg1, Arg2 ) const )
	{
		return Function(fn, &CallMethod<FunctionType, FunctionPtr, R, C, Arg1, Arg2> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1, typename Arg2, typename Arg3>
	Function BuildFunction(R (C::*fn)( Arg1, Arg2, Arg3 ) const )
	{
		return Function(fn, &CallMethod<FunctionType, FunctionPtr, R, C, Arg1, Arg2, Arg3> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Function BuildFunction(R (C::*fn)( Arg1, Arg2, Arg3, Arg4 ) const )
	{
		return Function(fn, &CallMethod<FunctionType, FunctionPtr, R, C, Arg1, Arg2, Arg3, Arg4> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename R, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Function BuildFunction(R (C::*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ) const )
	{
		return Function(fn, &CallMethod<FunctionType, FunctionPtr, R, C, Arg1, Arg2, Arg3, Arg4, Arg5> );
	}

	// Build methods without return value, non-const
	template <typename FunctionType, FunctionType FunctionPtr, typename C>
	Function BuildFunction(void (C::*fn)() )
	{
		return Function(fn, &CallMethodVoid<FunctionType, FunctionPtr, C> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1>
	Function BuildFunction(void (C::*fn)( Arg1 ) )
	{
		return Function(fn, &CallMethodVoid<FunctionType, FunctionPtr, C, Arg1> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1, typename Arg2>
	Function BuildFunction(void (C::*fn)( Arg1, Arg2 ) )
	{
		return Function(fn, &CallMethodVoid<FunctionType, FunctionPtr, C, Arg1, Arg2> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1, typename Arg2, typename Arg3>
	Function BuildFunction(void (C::*fn)( Arg1, Arg2, Arg3 ) )
	{
		return Function(fn, &CallMethodVoid<FunctionType, FunctionPtr, C, Arg1, Arg2, Arg3> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Function BuildFunction(void (C::*fn)( Arg1, Arg2, Arg3, Arg4 ) )
	{
		return Function(fn, &CallMethodVoid<FunctionType, FunctionPtr, C, Arg1, Arg2, Arg3, Arg4> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Function BuildFunction(void (C::*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ) )
	{
		return Function(fn, &CallMethodVoid<FunctionType, FunctionPtr, C, Arg1, Arg2, Arg3, Arg4, Arg5> );
	}

	// Build methods without return value, const
	template <typename FunctionType, FunctionType FunctionPtr, typename C>
	Function BuildFunction(void (C::*fn)() const )
	{
		return Function(fn, &CallMethodVoid<FunctionType, FunctionPtr, C> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1>
	Function BuildFunction(void (C::*fn)( Arg1 ) const )
	{
		return Function(fn, &CallMethodVoid<FunctionType, FunctionPtr, C, Arg1> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1, typename Arg2>
	Function BuildFunction(void (C::*fn)( Arg1, Arg2 ) const )
	{
		return Function(fn, &CallMethodVoid<FunctionType, FunctionPtr, C, Arg1, Arg2> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1, typename Arg2, typename Arg3>
	Function BuildFunction(void (C::*fn)( Arg1, Arg2, Arg3 ) const )
	{
		return Function(fn, &CallMethodVoid<FunctionType, FunctionPtr, C, Arg1, Arg2, Arg3> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	Function BuildFunction(void (C::*fn)( Arg1, Arg2, Arg3, Arg4 ) const )
	{
		return Function(fn, &CallMethodVoid<FunctionType, FunctionPtr, C, Arg1, Arg2, Arg3, Arg4> );
	}

	template <typename FunctionType, FunctionType FunctionPtr, typename C, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	Function BuildFunction(void (C::*fn)( Arg1, Arg2, Arg3, Arg4, Arg5 ) const )
	{
		return Function(fn, &CallMethodVoid<FunctionType, FunctionPtr, C, Arg1, Arg2, Arg3, Arg4, Arg5> );
	}

	// Call with return value
	template <typename Arg1>
	void Function::operator()( Variable& ret, Arg1 arg1 ) const
	{
		ASSERT( m_sig.ArgCount() == 1 );
		ASSERT( m_sig.GetArg( 0 ) == GET_TYPE( Arg1 ) );

		Variable argStack[1];

		new (argStack) Variable( arg1 );

		m_callHelper( &ret, m_context.Data(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2>
	void Function::operator()( Variable& ret, Arg1 arg1, Arg2 arg2 ) const
	{
		ASSERT( m_sig.ArgCount() == 2 );
		ASSERT( m_sig.GetArg( 0 ) == GET_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == GET_TYPE( Arg2 ) );

		Variable argStack[2];

		new (argStack)     Variable( arg1 );
		new (argStack + 1) Variable( arg2 );

		m_callHelper( &ret, m_context.Data(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2, typename Arg3>
	void Function::operator()( Variable& ret, Arg1 arg1, Arg2 arg2, Arg3 arg3 ) const
	{
		ASSERT( m_sig.ArgCount() == 3 );
		ASSERT( m_sig.GetArg( 0 ) == GET_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == GET_TYPE( Arg2 ) );
		ASSERT( m_sig.GetArg( 2 ) == GET_TYPE( Arg3 ) );

		Variable argStack[3];

		new (argStack)     Variable( arg1 );
		new (argStack + 1) Variable( arg2 );
		new (argStack + 2) Variable( arg3 );

		m_callHelper( &ret, m_context.Data(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void Function::operator()( Variable& ret, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4 ) const
	{
		ASSERT( m_sig.ArgCount() == 4 );
		ASSERT( m_sig.GetArg( 0 ) == GET_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == GET_TYPE( Arg2 ) );
		ASSERT( m_sig.GetArg( 2 ) == GET_TYPE( Arg3 ) );
		ASSERT( m_sig.GetArg( 3 ) == GET_TYPE( Arg4 ) );

		Variable argStack[4];

		new (argStack)     Variable( arg1 );
		new (argStack + 1) Variable( arg2 );
		new (argStack + 2) Variable( arg3 );
		new (argStack + 3) Variable( arg4 );

		m_callHelper( &ret, m_context.Data(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void Function::operator()( Variable& ret, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5 ) const
	{
		ASSERT( m_sig.ArgCount() == 5 );
		ASSERT( m_sig.GetArg( 0 ) == GET_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == GET_TYPE( Arg2 ) );
		ASSERT( m_sig.GetArg( 2 ) == GET_TYPE( Arg3 ) );
		ASSERT( m_sig.GetArg( 3 ) == GET_TYPE( Arg4 ) );
		ASSERT( m_sig.GetArg( 4 ) == GET_TYPE( Arg5 ) );

		Variable argStack[5];

		new (argStack)     Variable( arg1 );
		new (argStack + 1) Variable( arg2 );
		new (argStack + 2) Variable( arg3 );
		new (argStack + 3) Variable( arg4 );
		new (argStack + 4) Variable( arg5 );

		m_callHelper( &ret, m_context.Data(), argStack, m_sig.ArgCount() );
	}

	// Call without return value
	template <typename Arg1>
	void Function::operator()( Arg1 arg1 ) const
	{
		ASSERT( m_sig.ArgCount() == 1 );
		ASSERT( m_sig.GetArg( 0 ) == GET_TYPE( Arg1 ) );

		Variable argStack[1];

		new (argStack) Variable( arg1 );

		m_callHelper( nullptr, m_context.Data(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2>
	void Function::operator()( Arg1 arg1, Arg2 arg2 ) const
	{
		ASSERT( m_sig.ArgCount() == 2 );
		ASSERT( m_sig.GetArg( 0 ) == GET_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == GET_TYPE( Arg2 ) );

		Variable argStack[2];

		new (argStack)     Variable( arg1 );
		new (argStack + 1) Variable( arg2 );

		m_callHelper( nullptr, m_context.Data(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2, typename Arg3>
	void Function::operator()( Arg1 arg1, Arg2 arg2, Arg3 arg3 ) const
	{
		ASSERT( m_sig.ArgCount() == 3 );
		ASSERT( m_sig.GetArg( 0 ) == GET_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == GET_TYPE( Arg2 ) );
		ASSERT( m_sig.GetArg( 2 ) == GET_TYPE( Arg3 ) );

		Variable argStack[3];

		new (argStack)     Variable( arg1 );
		new (argStack + 1) Variable( arg2 );
		new (argStack + 2) Variable( arg3 );

		m_callHelper( nullptr, m_context.Data(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void Function::operator()( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4 ) const
	{
		ASSERT( m_sig.ArgCount() == 4 );
		ASSERT( m_sig.GetArg( 0 ) == GET_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == GET_TYPE( Arg2 ) );
		ASSERT( m_sig.GetArg( 2 ) == GET_TYPE( Arg3 ) );
		ASSERT( m_sig.GetArg( 3 ) == GET_TYPE( Arg4 ) );

		Variable argStack[4];

		new (argStack)     Variable( arg1 );
		new (argStack + 1) Variable( arg2 );
		new (argStack + 2) Variable( arg3 );
		new (argStack + 3) Variable( arg4 );

		m_callHelper( nullptr, m_context.Data(), argStack, m_sig.ArgCount() );
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	void Function::operator()( Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5 ) const
	{
		ASSERT( m_sig.ArgCount() == 5 );
		ASSERT( m_sig.GetArg( 0 ) == GET_TYPE( Arg1 ) );
		ASSERT( m_sig.GetArg( 1 ) == GET_TYPE( Arg2 ) );
		ASSERT( m_sig.GetArg( 2 ) == GET_TYPE( Arg3 ) );
		ASSERT( m_sig.GetArg( 3 ) == GET_TYPE( Arg4 ) );
		ASSERT( m_sig.GetArg( 4 ) == GET_TYPE( Arg5 ) );

		Variable argStack[5];

		new (argStack)     Variable( arg1 );
		new (argStack + 1) Variable( arg2 );
		new (argStack + 2) Variable( arg3 );
		new (argStack + 3) Variable( arg4 );
		new (argStack + 4) Variable( arg5 );

		m_callHelper( nullptr, m_context.Data(), argStack, m_sig.ArgCount() );
	}
}
