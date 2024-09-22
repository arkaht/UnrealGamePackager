#pragma once

#include "AutomationCommand.h"

class ZipBuildAutomationCommand : public AutomationCommand
{
public:
	void Initialize( const BuildSettings& BuildSettings ) override;
	void Run( const BuildSettings& BuildSettings ) override;

	virtual AutomationCommandRunTime GetRunTime() const override;;
};