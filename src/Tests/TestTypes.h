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

		DD_CLASS( Test::SimpleStruct )
		{
			DD_MEMBER( Test::SimpleStruct, Int );
			DD_MEMBER( Test::SimpleStruct, Str );
			DD_MEMBER( Test::SimpleStruct, Flt );
			DD_MEMBER( Test::SimpleStruct, Vec );
			DD_METHOD( Test::SimpleStruct, Double );
			DD_METHOD( Test::SimpleStruct, Multiply );
		}
	};

	struct NestedStruct
	{
		SimpleStruct Nested;
		int SecondInt;

		DD_CLASS( Test::NestedStruct )
		{
			DD_MEMBER( Test::NestedStruct, Nested );
			DD_MEMBER( Test::NestedStruct, SecondInt );
		}
	};
}