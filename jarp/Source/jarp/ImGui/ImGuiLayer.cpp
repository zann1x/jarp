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

		// TODO: Fill structure
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
		EventBus::Get().Register(EventCategoryApplication, this);
	}

	ImGuiLayer::~ImGuiLayer()
	{
		// TODO: The whole context should not be closed every time a layer is destroyed I guess
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
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
		// TODO: Render and Present data with command buffers, queues etc.
		ImGui::Render();
		
		// TODO: Record ImGui Data into a command buffer
		//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), CommandBuffer);
	}

	void ImGuiLayer::OnEvent(Event& E)
	{
		ImGui_ImplSDL2_ProcessEvent(static_cast<SDL_Event*>(E.GetNativeEvent()));

		// TODO: Handle swapchain recreation event (i.e. window resize/restore event)
		switch (E.GetEventType())
		{
			case EventTypeWindowResized:
			{
				//ImGui_ImplVulkan_SetMinImageCount(42);
				break;
			}
			case EventTypeWindowRestored:
			{
				//ImGui_ImplVulkan_SetMinImageCount(42);
				break;
			}
		}
	}

}
