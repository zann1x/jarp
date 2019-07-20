#include "jarppch.h"
#include "Command.h"

#include "jarp/Utils.hpp"

void DoNothingCommand::Execute()
{
}

MoveForwardCommand::MoveForwardCommand()
{
}

MoveForwardCommand::~MoveForwardCommand()
{
}

void MoveForwardCommand::Execute()
{
	CONSOLE_LOG("Moving forward");
}

MoveRightCommand::MoveRightCommand()
{
}

MoveRightCommand::~MoveRightCommand()
{
}

void MoveRightCommand::Execute()
{
	CONSOLE_LOG("Moving right");
}
