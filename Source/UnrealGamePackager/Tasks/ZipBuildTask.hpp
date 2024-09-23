#pragma once

#include "Task.hpp"

class ZipBuildTask : public Task
{
public:
	bool Initialize( BuildSettings& BuildSettings ) override;
	void Run( BuildSettings& BuildSettings ) override;

	virtual TaskRunTime GetRunTime() const override;
};