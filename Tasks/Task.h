#pragma once

#include <cstdint>

#include "mini/ini.h"

#include "BuildSettings.hpp"

enum class TaskRunTime : std::uint8_t
{
	PreBuild,
	PostBuild,
};

class Task
{
public:
	virtual void Initialize( const BuildSettings& BuildSettings ) = 0;
	virtual void Run( const BuildSettings& BuildSettings ) = 0;

	virtual TaskRunTime GetRunTime() const = 0;
};