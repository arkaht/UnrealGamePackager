#pragma once

#include "AutomationCommand.h"

class ProjectVersionUpdateAutomationCommand : public AutomationCommand
{
public:
	void Initialize( const BuildSettings& BuildSettings ) override;
	void Run( const BuildSettings& BuildSettings ) override;
	
	AutomationCommandRunTime GetRunTime() const override;
};