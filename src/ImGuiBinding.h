#pragma once

struct ImGuiContext;

namespace dd
{
	struct ImGuiBinding
	{
		static void Initialize();
		static void Shutdown();

		static void SetMouseHandling(bool handle);
		static bool IsMouseHandled();
		static void StartFrame(float delta_t);
		static void EndFrame();
	};
}