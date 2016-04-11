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

		BEGIN_TYPE( SimpleStruct )
			MEMBER( SimpleStruct, Int )
			MEMBER( SimpleStruct, Str )
			MEMBER( SimpleStruct, Flt )
			MEMBER( SimpleStruct, Vec )
			METHOD( SimpleStruct, Double )
			METHOD( SimpleStruct, Multiply )
		END_TYPE
	};

	struct NestedStruct
	{
		SimpleStruct Nested;
		int SecondInt;

		BEGIN_TYPE( NestedStruct )
			MEMBER( NestedStruct, Nested )
			MEMBER( NestedStruct, SecondInt )
		END_TYPE
	};
}