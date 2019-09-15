#include "jarppch.h"
#include "Application.h"

#include "Time.h"

#include "jarp/Events/ApplicationEvent.h"
#include "Platform/VulkanRHI/TempVulkanApplication.h"

namespace jarp {

	Application* Application::Instance = nullptr;

	Application::Application()
	{
		JARP_CORE_ASSERT(!Instance, "Application instance already exists!");
		Instance = this;

		Log::Init();

		pWindow = std::unique_ptr<Window>(Window::Create());

		EventBus::Get().Register(EventTypeWindowClosed, this);
	}

	Application::~Application()
	{
		EventBus::Get().Deregister(EventTypeWindowClosed, this);
	}

	void Application::Run()
	{
		bIsRunning = true;

		auto CurrentFPSTime = Time::GetTicks();
		auto LastFPSTime = CurrentFPSTime;
		uint32_t FPSCount = 0;

		auto CurrentFrameTime = Time::GetTicks();
		auto LastFrameTime = CurrentFrameTime;
		uint32_t DeltaFrameTime;

		TempVulkanApplication Renderer;
		Renderer.StartVulkan();

		while (bIsRunning)
		{
			// Calculate frames per second
			CurrentFPSTime = Time::GetTicks();
			++FPSCount;
			if (CurrentFPSTime > LastFPSTime + 1000)
			{
				JARP_CORE_TRACE("{0} fps", FPSCount);
				LastFPSTime = CurrentFPSTime;
				FPSCount = 0;
			}

			// Calculate frame time for correct timestep at drawing, input etc.
			CurrentFrameTime = Time::GetTicks();
			DeltaFrameTime = CurrentFrameTime - LastFrameTime;
			LastFrameTime = CurrentFrameTime;

			// Update everything
			Renderer.Render(DeltaFrameTime);
			pWindow->Update(DeltaFrameTime);
		}

		Renderer.ShutdownVulkan();
	}

	void Application::OnEvent(Event& E)
	{
		JARP_CORE_TRACE("On event (application): {0}", E.GetName());

		switch (E.GetEventType())
		{
			case EventTypeWindowClosed:
			{
				bIsRunning = false;
				E.bIsHandled = true;
				break;
			}
		}
	}

}
