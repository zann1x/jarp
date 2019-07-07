#pragma once

#include "Command.h"

class InputHandler
{
public:
	InputHandler();
	~InputHandler();

	void HandleInput(union SDL_Event& Event);

private:
	MoveForwardCommand MoveForward;
	MoveRightCommand MoveRight;
};
