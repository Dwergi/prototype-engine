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

		DD_CLASS( dd::SimpleStruct )
		{
			DD_MEMBER( dd::SimpleStruct, Int );
			DD_MEMBER( dd::SimpleStruct, Str );
			DD_MEMBER( dd::SimpleStruct, Flt );
			DD_MEMBER( dd::SimpleStruct, Vec );
			DD_METHOD( dd::SimpleStruct, Double );
			DD_METHOD( dd::SimpleStruct, Multiply );
		}
	};

	struct NestedStruct
	{
		SimpleStruct Nested;
		int SecondInt;

		DD_CLASS( dd::NestedStruct )
		{
			DD_MEMBER( dd::NestedStruct, Nested );
			DD_MEMBER( dd::NestedStruct, SecondInt );
		}
	};
}