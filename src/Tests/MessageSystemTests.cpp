//
// MessageSystemTests.cpp - Tests for MessageSystem.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PrecompiledHeader.h"

#include "catch/catch.hpp"

#include "MessageSystem.h"

bool called = false;

uint InvalidMessageID = ((uint) -1) - 100;
uint TestMessageID = ((uint) -1) - 1;

struct TestMessage : public dd::Message
{
	uint Payload;

	BEGIN_TYPE( TestMessage )
		PARENT( dd::Message );
	END_TYPE
};

dd::Message* Received;

void TestFunction( dd::Message* msg )
{
	Received = msg;
}

dd::Message* Received2;

void TestFunction2( dd::Message* msg )
{
	Received2 = msg;
}

TEST_CASE( "[MessageSystem]" )
{
	REGISTER_TYPE( TestMessage );

	dd::MessageSystem system;
	dd::Function f = FUNCTION( TestFunction );
	Received = nullptr;

	SECTION( "Subscribe/Unsubscribe" )
	{
		REQUIRE( system.GetTotalSubscriberCount() == 0 );
		REQUIRE( system.GetSubscriberCount( TestMessageID ) == 0 );

		dd::MessageSubscription subbed = system.Subscribe( TestMessageID, f );

		REQUIRE( system.GetSubscriberCount( TestMessageID ) == 1 );
		REQUIRE( system.GetTotalSubscriberCount() == 1 );

		system.Unsubscribe( subbed );

		REQUIRE( system.GetSubscriberCount( TestMessageID ) == 0 );
		REQUIRE( system.GetTotalSubscriberCount() == 0 );
	}

	SECTION( "Send Valid" )
	{
		system.Subscribe( TestMessageID, f );

		TestMessage msg;
		msg.Type = TestMessageID;
		msg.Payload = 50;

		system.Send( &msg );

		REQUIRE( Received == &msg );

		TestMessage* received = static_cast<TestMessage*>( Received );

		REQUIRE( received != nullptr );
		REQUIRE( received->Payload == 50 );
	}

	SECTION( "Send No Receivers" )
	{
		system.Subscribe( TestMessageID, f );

		TestMessage msg;
		msg.Type = InvalidMessageID;
		msg.Payload = 50;

		system.Send( &msg );

		REQUIRE( Received == nullptr );
	}

	SECTION( "Send Multiple Receivers" )
	{
		system.Subscribe( TestMessageID, f );

		dd::Function f2 = FUNCTION( TestFunction2 );
		Received2 = nullptr;

		dd::MessageSubscription sub = system.Subscribe( TestMessageID, f2 );

		TestMessage msg;
		msg.Type = TestMessageID;
		msg.Payload = 50;

		system.Send( &msg );

		REQUIRE( Received == &msg );
		REQUIRE( Received2 == &msg );

		system.Unsubscribe( sub );

		Received = nullptr;
		Received2 = nullptr;

		system.Send( &msg );

		REQUIRE( Received == &msg );
		REQUIRE( Received2 == nullptr );
	}
}