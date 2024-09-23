#pragma once

#include "Task.hpp"

class ProjectVersionUpdateTask : public Task
{
public:
	bool Initialize( BuildSettings& BuildSettings ) override;
	void Run( BuildSettings& BuildSettings ) override;
	
	TaskRunTime GetRunTime() const override;
};