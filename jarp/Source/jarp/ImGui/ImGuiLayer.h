#pragma once

#include "jarp/Events/EventListener.h"

namespace jarp {
	
	class ImGuiLayer : public EventListener
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void Begin();
		void Render();
		void End();

		virtual void OnEvent(Event& E) override;
	};

}
