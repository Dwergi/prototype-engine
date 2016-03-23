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
			MEMBER( Int );
			MEMBER( Str );
			MEMBER( Flt );
			MEMBER( Vec );
			METHOD( Double );
			METHOD( Multiply );
		END_TYPE
	};

	struct NestedStruct
	{
		SimpleStruct Nested;
		int SecondInt;

		BEGIN_TYPE( NestedStruct )
			MEMBER( Nested );
			MEMBER( SecondInt );
		END_TYPE
	};
}