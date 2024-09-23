#pragma once

#include "Task.h"

class ZipBuildTask : public Task
{
public:
	void Initialize( const BuildSettings& BuildSettings ) override;
	void Run( const BuildSettings& BuildSettings ) override;

	virtual TaskRunTime GetRunTime() const override;;
};