#include "ZipBuildAutomationCommand.h"

#include <string>
#include <stdlib.h>

void ZipBuildAutomationCommand::Initialize( const BuildSettings& BuildSettings )
{
}

void ZipBuildAutomationCommand::Run( const BuildSettings& BuildSettings )
{
	printf( "Zipping build directory...\n" );

	String DriveName = BuildSettings.ArchiveDirectoryPath.root_name().string();

	String Command = DriveName + " && "
		+ "cd " + BuildSettings.ArchiveDirectoryPath.string() + " && "
		+ "tar -c -f Build.zip Windows";

	int Status = system( Command.c_str() );
	printf( "Zipping command finished with status: %d.\n", Status );
}

AutomationCommandRunTime ZipBuildAutomationCommand::GetRunTime() const
{
	return AutomationCommandRunTime::PostBuild;
}
