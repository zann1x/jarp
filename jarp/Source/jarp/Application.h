#pragma once

#include "jarp/Core.h"
#include "jarp/Window.h"
#include "jarp/Events/Event.h"
#include "jarp/Events/EventListener.h"

namespace jarp {

	class Application : public EventListener
	{
	public:
		Application();
		virtual ~Application();

		static inline Application& Get() { return *Instance; }
		inline Window& GetWindow() { return *pWindow; }

		void Run();

		virtual void OnEvent(Event& E) override;

	private:
		static Application* Instance;
		bool bIsRunning = false;

		std::unique_ptr<Window> pWindow;
	};

	// To be defined in Client
	Application* CreateApplication();

}
