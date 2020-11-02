#include "PCH.h"
#include "ComponentBuffer.h"
#include "UpdateData.h"
#include "System.h"
#include "SystemsManager.h"
#include "SystemsSorting.h"

#include "catch2/catch.hpp"

struct FirstComponent
{
	int FirstValue { -100 };

	DD_BEGIN_CLASS(FirstComponent)
		DD_COMPONENT();
	DD_END_CLASS()
};

struct SecondComponent
{
	int SecondValue { 0 };

	DD_BEGIN_CLASS(SecondComponent)
		DD_COMPONENT();
	DD_END_CLASS()
};

struct ThirdComponent
{
	int ThirdValue { 0 };

	DD_BEGIN_CLASS(ThirdComponent)
		DD_COMPONENT();
	DD_END_CLASS()
};

DD_COMPONENT_CPP(FirstComponent);
DD_COMPONENT_CPP(SecondComponent);
DD_COMPONENT_CPP(ThirdComponent);

struct TestSystem : ddc::System
{
	TestSystem() :
		System("TestSystem")
	{
		RequireRead<FirstComponent>();
		RequireWrite<SecondComponent>();
	}

	virtual void Update(ddc::UpdateData& data) override
	{
		const auto& buffer = data.Data();

		auto read = buffer.Read<FirstComponent>();
		auto write = buffer.Write<SecondComponent>();

		DD_ASSERT(read.Size() == write.Size());

		for (size_t i = 0; i < read.Size(); ++i)
		{
			const FirstComponent& cmp = read[i];
			write[i].SecondValue = cmp.FirstValue;
		}
	}
};

struct DependentSystem : ddc::System
{
	DependentSystem() :
		System("DependentSystem")
	{
		RequireRead<SecondComponent>();
		RequireWrite<ThirdComponent>();
	}

	virtual void Update(ddc::UpdateData& data) override
	{
		const auto& buffer = data.Data();

		auto read = buffer.Read<SecondComponent>();
		auto write = buffer.Write<ThirdComponent>();

		DD_ASSERT(read.Size() == write.Size());

		for (size_t i = 0; i < read.Size(); ++i)
		{
			const SecondComponent& cmp = read[i];
			write[i].ThirdValue = cmp.SecondValue;
		}
	}
};

struct ReaderSystem : ddc::System
{
	ReaderSystem() :
		System("ReaderSystem")
	{
		RequireRead<ThirdComponent>();
	}

	virtual void Update(ddc::UpdateData& data) override
	{
		const auto& buffer = data.Data();

		auto read = buffer.Read<ThirdComponent>();

		for (size_t i = 0; i < read.Size(); ++i)
		{
			const ThirdComponent& cmp = read[i];
			int x = cmp.ThirdValue * cmp.ThirdValue;
		}
	}
};

struct OnlyReaderSystem : ddc::System
{
	OnlyReaderSystem() :
		System("OnlyReaderSystem")
	{
		RequireRead<FirstComponent>();
		RequireRead<SecondComponent>();
	}

	virtual void Update(ddc::UpdateData& data) override
	{
		const auto& buffer = data.Data();

		auto read1 = buffer.Read<FirstComponent>();
		auto read2 = buffer.Read<SecondComponent>();

		for (size_t i = 0; i < read1.Size(); ++i)
		{
			const FirstComponent& cmp1 = read1[i];
			const SecondComponent& cmp2 = read2[i];
			int x = cmp1.FirstValue * cmp2.SecondValue;
		}
	}
};

struct OnlyWriterSystem : ddc::System
{
	OnlyWriterSystem() :
		System("OnlyWriterSystem")
	{
		RequireWrite<SecondComponent>();
		RequireWrite<ThirdComponent>();
	}

	virtual void Update(ddc::UpdateData& data) override {}
};

TEST_CASE("EntityLayer")
{
	ddc::EntityLayer::DestroyAllLayers();

	ddc::EntityLayer layer("test");

	ddc::Entity a = layer.CreateEntity();
	REQUIRE(a.ID == 0);

	ddc::Entity b = layer.CreateEntity();
	REQUIRE(b.ID == 1);

	ddc::Entity c = layer.CreateEntity();
	REQUIRE(c.ID == 2);

	layer.DestroyEntity(a);
	layer.Update();

	ddc::Entity a2 = layer.CreateEntity();
	REQUIRE(a2.ID == a.ID);
	REQUIRE(a2.Version == 2);

	layer.DestroyEntity(b);
	layer.Update();

	ddc::Entity b2 = layer.CreateEntity();
	REQUIRE(b2.ID == b.ID);
	REQUIRE(b2.Version == 2);
}

TEST_CASE("Component")
{
	ddc::EntityLayer::DestroyAllLayers();

	ddc::EntityLayer layer("test");
	ddc::Entity a = layer.CreateEntity();

	const dd::TypeInfo* type = DD_FIND_TYPE(FirstComponent);

	// component shouldn't be registered yet until a system that uses it is created
	REQUIRE(type->ComponentID() == dd::INVALID_COMPONENT);

	TestSystem system;

	REQUIRE(type->ComponentID() != dd::INVALID_COMPONENT);

	layer.Update();

	bool found = layer.Has<FirstComponent>(a);
	REQUIRE(found == false);

	FirstComponent& cmp = layer.Add<FirstComponent>(a);
	REQUIRE(cmp.FirstValue == -100);

	cmp.FirstValue = 5;

	const FirstComponent& cmp2 = *layer.Access<FirstComponent>(a);
	REQUIRE(cmp2.FirstValue == 5);
	REQUIRE(cmp.FirstValue == cmp2.FirstValue);

	REQUIRE(layer.Has<FirstComponent>(a));

	layer.Remove<FirstComponent>(a);
	REQUIRE_FALSE(layer.Has<FirstComponent>(a));
}

TEST_CASE("Update System")
{
	ddc::EntityLayer::DestroyAllLayers();

	TestSystem system;

	ddc::EntityLayer layer("test");

	ddc::SystemsManager system_mgr;
	system_mgr.Register(system);
	system_mgr.Initialize(layer);

	for (int i = 0; i < 8; ++i)
	{
		ddc::Entity e = layer.CreateEntity();

		FirstComponent& simple = layer.Add<FirstComponent>(e);
		simple.FirstValue = i;

		SecondComponent& other = layer.Add<SecondComponent>(e);
		other.SecondValue = -1;
	}

	layer.Update();
	system_mgr.Update(layer, 0);

	for (int i = 0; i < 8; ++i)
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 1;

		FirstComponent& simple = *layer.Access<FirstComponent>(e);
		REQUIRE(simple.FirstValue == e.ID);

		SecondComponent& other = *layer.Access<SecondComponent>(e);
		REQUIRE(other.SecondValue == e.ID);
	}
}

TEST_CASE("Update With Discontinuity")
{
	ddc::EntityLayer::DestroyAllLayers();

	TestSystem system;

	ddc::EntityLayer layer("test");

	ddc::SystemsManager system_mgr;
	system_mgr.Register(system);
	system_mgr.Initialize(layer);

	for (int i = 0; i < 5; ++i)
	{
		ddc::Entity e = layer.CreateEntity();

		if (i == 2)
			continue;

		FirstComponent& simple = layer.Add<FirstComponent>(e);
		simple.FirstValue = i;

		SecondComponent& other = layer.Add<SecondComponent>(e);
		other.SecondValue = -1;
	}

	layer.Update();
	system_mgr.Update(layer, 0);

	for (int i = 0; i < 5; ++i)
	{
		if (i == 2)
			continue;

		ddc::Entity e;
		e.ID = i;
		e.Version = 1;

		FirstComponent& simple = *layer.Access<FirstComponent>(e);
		REQUIRE(simple.FirstValue == e.ID);

		SecondComponent& other = *layer.Access<SecondComponent>(e);
		REQUIRE(other.SecondValue == e.ID);
	}
}

TEST_CASE("Update Multiple Systems")
{
	ddc::EntityLayer::DestroyAllLayers();

	TestSystem a;
	DependentSystem b;

	ddc::EntityLayer layer("test");

	ddc::SystemsManager system_mgr;
	system_mgr.Register(a);
	system_mgr.Register(b);
	system_mgr.Initialize(layer);

	for (int i = 0; i < 4; ++i)
	{
		ddc::Entity e = layer.CreateEntity();

		FirstComponent& first = layer.Add<FirstComponent>(e);
		first.FirstValue = i;

		SecondComponent& second = layer.Add<SecondComponent>(e);
		second.SecondValue = -1;

		ThirdComponent& third = layer.Add<ThirdComponent>(e);
		third.ThirdValue = -1;
	}

	layer.Update();
	system_mgr.Update(layer, 0);

	for (int i = 0; i < 4; ++i)
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 1;

		SecondComponent& second = *layer.Access<SecondComponent>(e);
		REQUIRE(second.SecondValue == e.ID);
	}

	system_mgr.Update(layer, 0);

	for (int i = 0; i < 4; ++i)
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 1;

		ThirdComponent& third = *layer.Access<ThirdComponent>(e);
		REQUIRE(third.ThirdValue == e.ID);
	}
}

TEST_CASE("Schedule Systems By Dependency")
{
	ddc::EntityLayer::DestroyAllLayers();

	SECTION("Simple")
	{
		TestSystem a;

		TestSystem b;
		b.DependsOn(a);

		ddc::System* systems[] = { &b, &a };

		dd::Span<ddc::System*> span_systems(systems);

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies(span_systems, ordered);

		REQUIRE(ordered[0].m_system == &a);
		REQUIRE(ordered[1].m_system == &b);
	}

	SECTION("Chain")
	{
		TestSystem a;

		TestSystem b;
		b.DependsOn(a);

		TestSystem c;
		c.DependsOn(b);

		ddc::System* systems[] = { &c, &b, &a };

		dd::Span<ddc::System*> span_systems(systems);

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies(span_systems, ordered);

		REQUIRE(ordered[0].m_system == &a);
		REQUIRE(ordered[1].m_system == &b);
		REQUIRE(ordered[2].m_system == &c);
	}

	SECTION("Diamond")
	{
		TestSystem a;

		TestSystem b;
		b.DependsOn(a);

		TestSystem c;
		c.DependsOn(a);

		TestSystem d;
		d.DependsOn(b);
		d.DependsOn(c);

		ddc::System* systems[] = { &d, &c, &a, &b };

		dd::Span<ddc::System*> span_systems(systems, 4);

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies(span_systems, ordered);

		REQUIRE(ordered[0].m_system == &a);
		REQUIRE(ordered[1].m_system == &b);
		REQUIRE(ordered[2].m_system == &c);
		REQUIRE(ordered[3].m_system == &d);
	}
}

TEST_CASE("Update With Tree Scheduling")
{
	ddc::EntityLayer::DestroyAllLayers();

	SECTION("Simple")
	{
		TestSystem a;

		TestSystem b;
		b.DependsOn(a);

		ddc::System* systems[] = { &b, &a };

		dd::Span<ddc::System*> span_systems(systems);

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies(span_systems, ordered);

		REQUIRE(ordered[0].m_system == &a);
		REQUIRE(ordered[1].m_system == &b);

		ddc::EntityLayer layer("test");

		ddc::SystemsManager system_mgr;
		system_mgr.Register(a);
		system_mgr.Register(b);
		system_mgr.Initialize(layer);

		system_mgr.Update(layer, 0);
	}

	SECTION("Multiple Roots")
	{
		TestSystem a;
		TestSystem b;

		TestSystem c;
		c.DependsOn(a);
		c.DependsOn(b);

		ddc::System* systems[] = { &c, &a, &b };

		dd::Span<ddc::System*> span_systems(systems);

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies(span_systems, ordered);

		REQUIRE(ordered[0].m_system == &a);
		REQUIRE(ordered[1].m_system == &b);
		REQUIRE(ordered[2].m_system == &c);

		ddc::EntityLayer layer("test");

		ddc::SystemsManager system_mgr;
		system_mgr.Register(a);
		system_mgr.Register(b);
		system_mgr.Initialize(layer);

		system_mgr.Update(layer, 0);
	}

	SECTION("Diamond")
	{
		TestSystem a;

		TestSystem b;
		TestSystem c;
		c.DependsOn(a);
		c.DependsOn(b);

		TestSystem d;
		d.DependsOn(c);

		ddc::System* systems[] = { &d, &c, &a, &b };

		dd::Span<ddc::System*> span_systems(systems);

		std::vector<ddc::SystemNode> ordered;
		ddc::OrderSystemsByDependencies(span_systems, ordered);

		REQUIRE(ordered[0].m_system == &a);
		REQUIRE(ordered[1].m_system == &b);
		REQUIRE(ordered[2].m_system == &c);
		REQUIRE(ordered[3].m_system == &d);

		ddc::EntityLayer layer("test");

		ddc::SystemsManager system_mgr;
		system_mgr.Register(a);
		system_mgr.Register(b);
		system_mgr.Register(c);
		system_mgr.Initialize(layer);

		system_mgr.Update(layer, 0);
	}
}

TEST_CASE("Full Update Loop")
{
	ddc::EntityLayer::DestroyAllLayers();

	TestSystem a;
	DependentSystem b;
	ReaderSystem c;

	ddc::EntityLayer layer("test");

	ddc::SystemsManager system_mgr;
	system_mgr.Register(c);
	system_mgr.Register(b);
	system_mgr.Register(a);
	system_mgr.Initialize(layer);

	BENCHMARK("Create layer")
	{
		for (int i = 0; i < 1000; ++i)
		{
			ddc::Entity e = layer.CreateEntity();
			FirstComponent& first = layer.Add<FirstComponent>(e);
			first.FirstValue = i;

			SecondComponent& second = layer.Add<SecondComponent>(e);
			second.SecondValue = 0;

			ThirdComponent& third = layer.Add<ThirdComponent>(e);
			third.ThirdValue = 0;
		}
	}

	BENCHMARK("Update")
	{
		for (int i = 0; i < 100; ++i)
		{
			layer.Update();
			system_mgr.Update(layer, 0.0f);
		}
	}

	for (int i = 0; i < 1000; ++i)
	{
		ddc::Entity e;
		e.ID = i;
		e.Version = 1;

		const SecondComponent* second = layer.Get<SecondComponent>(e);
		REQUIRE(second->SecondValue == i);

		const ThirdComponent* third = layer.Get<ThirdComponent>(e);
		REQUIRE(third->ThirdValue == i);
	}
}