//
// ASInternal.h - Helpers around AngelScript arguments and return values, allowing you to set them relatively simply.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	namespace ASInternal
	{
		template <typename T>
		struct ReturnValue
		{
			static void Get( asIScriptContext* context, T& ret )
			{
				DD_ASSERT( "ReturnValue not implemented for this type! Function declaration: %s", context->GetFunction()->GetDeclaration() );
			}
		};

		template <>
		struct ReturnValue<int>
		{
			static int Get( asIScriptContext* context, int& ret )
			{
				ret = context->GetReturnDWord();
			}
		};

		template <>
		struct ReturnValue<char>
		{
			static int Get( asIScriptContext* context, char& ret )
			{
				ret = context->GetReturnByte();
			}
		};

		template <>
		struct ReturnValue<byte>
		{
			static int Get( asIScriptContext* context, byte& ret )
			{
				ret = (byte) context->GetReturnByte();
			}
		};

		template <>
		struct ReturnValue<float>
		{
			static int Get( asIScriptContext* context, float& ret )
			{
				ret = (float) context->GetReturnFloat();
			}
		};

		template <>
		struct ReturnValue<double>
		{
			static int Get( asIScriptContext* context, double& ret )
			{
				ret = (double) context->GetReturnDouble();
			}
		};

		template <>
		struct ReturnValue<int16>
		{
			static int Get( asIScriptContext* context, int16& ret )
			{
				ret = (int16) context->GetReturnWord();
			}
		};

		template <>
		struct ReturnValue<uint16>
		{
			static int Get( asIScriptContext* context, uint16& ret )
			{
				ret = (uint16) context->GetReturnWord();
			}
		};

		template <>
		struct ReturnValue<int64>
		{
			static int Get( asIScriptContext* context, int64& ret )
			{
				ret = (int64) context->GetReturnWord();
			}
		};

		template <>
		struct ReturnValue<uint64>
		{
			static int Get( asIScriptContext* context, uint64& ret )
			{
				ret = (uint64) context->GetReturnWord();
			}
		};

		template <typename T>
		struct ASArgument
		{
			static void set( asIScriptContext* context, int arg, const T& val )
			{
				DD_ASSERT( "ASArgument not implemented for this type! Function declaration: %s", context->GetFunction()->GetDeclaration() );
			}
		};

		template <>
		struct ASArgument<int>
		{
			static void set( asIScriptContext* context, int arg, const int& val )
			{
				context->SetArgDWord( arg, val );
			}
		};

		template <>
		struct ASArgument<int64>
		{
			static void set( asIScriptContext* context, int arg, const int64& val )
			{
				context->SetArgQWord( arg, val );
			}
		};

		template <>
		struct ASArgument<int16>
		{
			static void set( asIScriptContext* context, int arg, const int16& val )
			{
				context->SetArgWord( arg, val );
			}
		};

		template <>
		struct ASArgument<char>
		{
			static void set( asIScriptContext* context, int arg, const char& val )
			{
				context->SetArgByte( arg, val );
			}
		};

		template <>
		struct ASArgument<float>
		{
			static void set( asIScriptContext* context, int arg, const float& val )
			{
				context->SetArgFloat( arg, val );
			}
		};

		template <>
		struct ASArgument<double>
		{
			static void set( asIScriptContext* context, int arg, const double& val )
			{
				context->SetArgDouble( arg, val );
			}
		};
	}
}
