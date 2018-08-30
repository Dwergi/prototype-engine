//
// TestTypes.h - Shared test types.
// Copyright (C) Sebastian Nordgren 
// October 10th 2015
//

#pragma once

namespace Test
{
	struct SimpleStruct
	{
		int Int;
		dd::String32 Str;
		float Flt;
		dd::Vector<int> Vec;

		void Double() const;
		int Multiply( int x );

		bool operator==( const SimpleStruct& other ) const;

		DD_BEGIN_TYPE( SimpleStruct )
			DD_MEMBER( SimpleStruct, Int )
			DD_MEMBER( SimpleStruct, Str )
			DD_MEMBER( SimpleStruct, Flt )
			DD_MEMBER( SimpleStruct, Vec )
			DD_METHOD( SimpleStruct, Double )
			DD_METHOD( SimpleStruct, Multiply )
		DD_END_TYPE
	};

	struct NestedStruct
	{
		SimpleStruct Nested;
		int SecondInt;

		DD_BEGIN_TYPE( NestedStruct )
			DD_MEMBER( NestedStruct, Nested )
			DD_MEMBER( NestedStruct, SecondInt )
		DD_END_TYPE
	};
}