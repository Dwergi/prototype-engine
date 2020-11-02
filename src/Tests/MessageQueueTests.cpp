//
// MessageQueueTests.cpp - Tests for MessageQueue.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PCH.h"

#include "catch2/catch.hpp"

#include "MessageQueue.h"

ddc::MessageType InvalidMessageID = ddc::MessageType::Unknown;
ddc::MessageType TestMessageID = ddc::MessageType::Register<int>("Test");

int Received;

void TestFunction(ddc::Message msg)
{
	Received = msg.GetPayload<int>();
}

int Received2;

void TestFunction2(ddc::Message msg)
{
	Received2 = msg.GetPayload<int>();
}

TEST_CASE("[MessageSystem]")
{
	DD_REGISTER_CLASS(ddc::Message);

	ddc::MessageQueue system;
	Received = 0;
	Received2 = 0;

	SECTION("Subscribe/Unsubscribe")
	{
		REQUIRE(system.GetTotalSubscriberCount() == 0);
		REQUIRE(system.GetSubscriberCount(TestMessageID) == 0);

		ddc::MessageSubscription subbed = system.Subscribe(TestMessageID, TestFunction);

		REQUIRE(system.GetSubscriberCount(TestMessageID) == 1);
		REQUIRE(system.GetTotalSubscriberCount() == 1);

		system.Unsubscribe(subbed);

		REQUIRE(system.GetSubscriberCount(TestMessageID) == 0);
		REQUIRE(system.GetTotalSubscriberCount() == 0);
	}

	SECTION("Send Valid")
	{
		system.Subscribe(TestMessageID, TestFunction);

		ddc::Message msg(TestMessageID);
		msg.SetPayload(50);

		system.Send(msg);
		system.Update();

		REQUIRE(Received == 50);
	}

	SECTION("Send No Receivers")
	{
		system.Subscribe(TestMessageID, TestFunction);

		ddc::Message msg(InvalidMessageID);

		system.Send(msg);
		system.Update();

		REQUIRE(Received == 0);
	}

	SECTION("Send Multiple Receivers")
	{
		system.Subscribe(TestMessageID, TestFunction);

		ddc::MessageSubscription sub = system.Subscribe(TestMessageID, TestFunction2);

		ddc::Message msg(TestMessageID);
		msg.SetPayload(50);

		system.Send(msg);
		system.Update();

		REQUIRE(Received == 50);
		REQUIRE(Received2 == 50);

		system.Unsubscribe(sub);

		Received = 0;
		Received2 = 0;

		system.Send(msg);
		system.Update();

		REQUIRE(Received == 50);
		REQUIRE(Received2 == 0);
	}
}