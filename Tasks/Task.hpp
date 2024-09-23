#pragma once

#include <cstdint>

#include "BuildSettings.hpp"

enum class TaskRunTime : std::uint8_t
{
	PreBuild,
	PostBuild,
};

class Task
{
public:
	virtual bool Initialize( BuildSettings& BuildSettings ) = 0;
	virtual void Run( BuildSettings& BuildSettings ) = 0;

	virtual TaskRunTime GetRunTime() const = 0;

public:
	bool bCanRun = true;
};