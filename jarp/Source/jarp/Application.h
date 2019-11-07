#pragma once

#include "jarp/Window.h"

#include "jarp/Events/EventListener.h"

namespace jarp {

	class Application : public EventListener
	{
	public:
		Application();
		virtual ~Application();

		static inline Application& Get() { return *s_Instance; }
		inline Window& GetWindow() const { return *m_Window; }

		void Run();

		virtual void OnEvent(Event& outEvent) override;

	private:
		static Application* s_Instance;
		bool m_bIsRunning = false;

		std::unique_ptr<Window> m_Window;
	};

	// To be defined in Client
	Application* CreateApplication();

}
