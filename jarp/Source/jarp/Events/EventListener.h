#pragma once

#include "Event.h"

namespace jarp {

	class EventListener
	{
	public:
		virtual void OnEvent(Event& event) = 0;
	};

}
