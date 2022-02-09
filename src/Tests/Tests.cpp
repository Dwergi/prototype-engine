//
// Tests.cpp - Test runner for Catch tests.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "Tests.h"

#include "DebugUI.h"
#include "File.h"
#include "JobSystem.h"

struct TestDebugUI : dd::IDebugUI
{
	void RegisterDebugPanel(dd::IDebugPanel& debug_panel) override {}
	void RenderDebugPanels() override {}

	void SetDraw(bool draw) override {}
	bool ShouldDraw() const override { return false; }

	void StartFrame(float delta_t) override {}
	void EndFrame() override {}

	void SetMouseCapture(bool capture) override {}
	bool HasMouseCapture() const override { return false; }

	bool IsMidFrame() const override { return false; }
	bool IsMidWindow() const override { return false; }
	void EndWindow() override {}
};

static TestInitializer s_instance;

TestInitializer::TestInitializer()
{
	dd::InitializeMemoryTracking();
	dd::SetAsMainThread();

	dd::TypeInfo::RegisterQueuedTypes();

	dd::Services::Register(new dd::JobSystem(std::thread::hardware_concurrency() - 1));
	dd::Services::RegisterInterface<dd::IDebugUI>(new TestDebugUI());
}