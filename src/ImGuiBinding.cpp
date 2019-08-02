#include "PCH.h"
#include "ImGuiBinding.h"

#include "IInputSource.h"
#include "IWindow.h"

#include "sfml/Window.hpp"

#include "imgui/imgui_impl_opengl3.h"

namespace dd
{
	static dd::Service<IWindow> s_window;
	static dd::Service<IInputSource> s_inputSource;

	ImGuiBinding::ImGuiBinding()
	{
		
	}

	void ImGuiBinding::Initialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		ImGui_ImplOpenGL3_Init("#version 130");
	}

	void ImGuiBinding::Shutdown()
	{

	}
}