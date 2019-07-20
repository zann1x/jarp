#pragma once

#include "Command.h"
#include "SDL.h"

namespace jarp {

	class InputHandler
	{
	public:
		InputHandler();
		~InputHandler();

		void HandleInput(SDL_Event& Event);

	private:
		MoveForwardCommand MoveForward;
		MoveRightCommand MoveRight;
	};

}
