#pragma once

struct ImGuiContext;

namespace dd
{
	struct ImGuiBinding
	{
		ImGuiBinding();

		void Initialize();
		void Shutdown();
	};
}