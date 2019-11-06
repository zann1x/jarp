#include "jarppch.h"

#include "Application.h"

#include "jarp/Core.h"
#include "jarp/Time.h"
#include "jarp/Window.h"
#include "jarp/Events/ApplicationEvent.h"
#include "Platform/VulkanRHI/TempVulkanApplication.h"

namespace jarp {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		JARP_CORE_ASSERT(!s_Instance, "Application instance already exists!");
		s_Instance = this;

		Log::Init();

		m_Window = std::unique_ptr<Window>(Window::Create());

		EventBus::Get().Register(EventTypeWindowClosed, this);
	}

	Application::~Application()
	{
		EventBus::Get().Deregister(EventTypeWindowClosed, this);
	}

	void Application::Run()
	{
		m_bIsRunning = true;

		auto currentFPSTime = Time::GetTicks();
		auto lastFPSTime = currentFPSTime;
		uint32_t frames = 0;

		auto currentFrameTime = Time::GetTicks();
		auto lastFrameTime = currentFrameTime;
		uint32_t deltaFrameTime;

		TempVulkanApplication Renderer;
		Renderer.StartVulkan();

		while (m_bIsRunning)
		{
			// Calculate frames per second
			currentFPSTime = Time::GetTicks();
			++frames;
			if (currentFPSTime > lastFPSTime + 1000)
			{
				JARP_CORE_TRACE("{0} fps", frames);
				lastFPSTime = currentFPSTime;
				frames = 0;
			}

			// Calculate frame time for correct timestep at drawing, input etc.
			currentFrameTime = Time::GetTicks();
			deltaFrameTime = currentFrameTime - lastFrameTime;
			lastFrameTime = currentFrameTime;

			// Update everything
			Renderer.Render(deltaFrameTime);
			m_Window->Update(deltaFrameTime);
		}

		Renderer.ShutdownVulkan();
	}

	void Application::OnEvent(Event& event)
	{
		JARP_CORE_TRACE("On event (application): {0}", event.GetName());

		switch (event.GetEventType())
		{
			case EventTypeWindowClosed:
			{
				m_bIsRunning = false;
				event.m_bIsHandled = true;
				break;
			}
		}
	}

}
