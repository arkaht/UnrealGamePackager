#include "ZipBuildTask.hpp"

#include <string>
#include <stdlib.h>

const std::string SECTION = "TaskSettings.ZipBuildSettings";

bool ZipBuildTask::Initialize( BuildSettings& BuildSettings )
{
	auto IsEnabled = BuildSettings.GetOrSet( SECTION, "bIsEnabled", "1" );
	return IsEnabled == "1";
}

void ZipBuildTask::Run( BuildSettings& BuildSettings )
{
	printf( "Zipping build directory...\n" );

	FilePath ArchiveDirectoryPath = BuildSettings.GetArchiveDirectoryPath();
	String DriveName = ArchiveDirectoryPath.root_name().string();

	String ZipFileName = BuildSettings.GetOrSet( SECTION, "ZipFileName", "Build.zip" );
	String BuildFolderName = BuildSettings.GetOrSet( SECTION, "BuildFolderName", "Windows" );

	String Command = DriveName + " && "
		+ "cd " + ArchiveDirectoryPath.string() + " && "
		+ "tar -c -f " + ZipFileName + " " + BuildFolderName;

	int Status = system( Command.c_str() );
	printf( "Zipping command finished with status: %d.\n", Status );
}

TaskRunTime ZipBuildTask::GetRunTime() const
{
	return TaskRunTime::PostBuild;
}
