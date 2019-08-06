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

		DD_BEGIN_CLASS( Test::SimpleStruct )
			DD_MEMBER( Int );
			DD_MEMBER( Str );
			DD_MEMBER( Flt );
			DD_MEMBER( Vec );
			DD_METHOD( Double );
			DD_METHOD( Multiply );
		DD_END_CLASS()
	};

	struct NestedStruct
	{
		SimpleStruct Nested;
		int SecondInt;

		DD_BEGIN_CLASS( Test::NestedStruct )
			DD_MEMBER( Nested );
			DD_MEMBER( SecondInt );
		DD_END_CLASS()
	};
}