#pragma once

#include "Task.h"

class ProjectVersionUpdateTask : public Task
{
public:
	void Initialize( const BuildSettings& BuildSettings ) override;
	void Run( const BuildSettings& BuildSettings ) override;
	
	TaskRunTime GetRunTime() const override;
};