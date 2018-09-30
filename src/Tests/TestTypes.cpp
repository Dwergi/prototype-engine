//
// TestTypes.h - Shared test types.
// Copyright (C) Sebastian Nordgren 
// October 10th 2015
//

#include "PCH.h"
#include "TestTypes.h"


void Test::SimpleStruct::Double() const
{

}

int Test::SimpleStruct::Multiply( int x )
{
	return Int * x;
}

bool Test::SimpleStruct::operator==( const Test::SimpleStruct& other ) const
{
	return Int == other.Int && Flt == other.Flt && Str == other.Str && Vec == other.Vec;
}