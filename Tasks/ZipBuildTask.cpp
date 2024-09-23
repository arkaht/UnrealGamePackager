#include "ZipBuildTask.hpp"

#include <string>
#include <stdlib.h>

bool ZipBuildTask::Initialize( BuildSettings& BuildSettings )
{
	return true;
}

void ZipBuildTask::Run( BuildSettings& BuildSettings )
{
	printf( "Zipping build directory...\n" );

	FilePath ArchiveDirectoryPath = BuildSettings.GetArchiveDirectoryPath();
	String DriveName = ArchiveDirectoryPath.root_name().string();

	String Command = DriveName + " && "
		+ "cd " + ArchiveDirectoryPath.string() + " && "
		+ "tar -c -f Build.zip Windows";

	int Status = system( Command.c_str() );
	printf( "Zipping command finished with status: %d.\n", Status );
}

TaskRunTime ZipBuildTask::GetRunTime() const
{
	return TaskRunTime::PostBuild;
}
