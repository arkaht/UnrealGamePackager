#pragma once

#include <cstdint>

#include "mini/ini.h"

#include "BuildSettings.hpp"

enum class AutomationCommandRunTime : std::uint8_t
{
	PreBuild,
	PostBuild,
};

class AutomationCommand
{
public:
	virtual void Initialize( const BuildSettings& BuildSettings ) = 0;
	virtual void Run( const BuildSettings& BuildSettings ) = 0;

	virtual AutomationCommandRunTime GetRunTime() const = 0;
};