#pragma once

class ICommand
{
public:
	virtual ~ICommand() {}
	virtual void Execute() = 0;
};

class DoNothingCommand : public ICommand
{
public:
	virtual void Execute() override;
};

class MoveForwardCommand : public ICommand
{
public:
	MoveForwardCommand();
	virtual ~MoveForwardCommand() override;

	virtual void Execute() override;
};

class MoveRightCommand : public ICommand
{
public:
	MoveRightCommand();
	virtual ~MoveRightCommand() override;

	virtual void Execute() override;
};
