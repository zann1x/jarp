#include "jarppch.h"
#include "InputHandler.h"

#include "SDL.h"

InputHandler::InputHandler()
{
}

InputHandler::~InputHandler()
{
}

void InputHandler::HandleInput(union SDL_Event& Event)
{
	switch (Event.key.keysym.sym)
	{
	case SDLK_w:
		MoveForward.Execute();
		break;
	case SDLK_d:
		MoveRight.Execute();
		break;
	}
}
