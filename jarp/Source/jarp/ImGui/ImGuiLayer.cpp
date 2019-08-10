#include "jarppch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "examples/imgui_impl_sdl.h"
#include "examples/imgui_impl_vulkan.h"

#include "jarp/Application.h"
#include "jarp/Events/EventBus.h"

namespace jarp {

	ImGuiLayer::ImGuiLayer()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		
		ImGuiIO& IO = ImGui::GetIO();
		IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		ImGui::StyleColorsDark();

		ImGui_ImplSDL2_InitForVulkan(static_cast<SDL_Window*>(Application::Get().GetWindow().GetNativeWindow()));

		ImGui_ImplVulkan_InitInfo InitInfo = {};
		//InitInfo.Instance;
		//InitInfo.PhysicalDevice;
		//InitInfo.Device;
		//InitInfo.QueueFamily;
		//InitInfo.Queue;
		//InitInfo.PipelineCache;
		//InitInfo.DescriptorPool;
		//InitInfo.MinImageCount;          // >= 2
		//InitInfo.ImageCount;             // >= MinImageCount
		//InitInfo.MSAASamples;   // >= VK_SAMPLE_COUNT_1_BIT
		InitInfo.Allocator = nullptr;
		InitInfo.CheckVkResultFn = nullptr;
		ImGui_ImplVulkan_Init(&InitInfo, VkRenderPass());

		EventBus::Get().Register(EventCategoryInput, this);
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(static_cast<SDL_Window*>(Application::Get().GetWindow().GetNativeWindow()));
		ImGui::NewFrame();
	}

	void ImGuiLayer::Render()
	{
		ImGui::ShowDemoWindow();
	}

	void ImGuiLayer::End()
	{
		// TODO
		ImGui::Render();
	}

	void ImGuiLayer::OnEvent(Event& E)
	{
		ImGui_ImplSDL2_ProcessEvent(static_cast<SDL_Event*>(E.GetNativeEvent()));
	}

}
