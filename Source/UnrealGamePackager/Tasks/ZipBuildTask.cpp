#include "ZipBuildTask.hpp"

#include <string>
#include <stdlib.h>

#include "fmt/format.h"

const std::string SECTION = "TaskSettings.ZipBuildSettings";

bool ZipBuildTask::Initialize( BuildSettings& BuildSettings )
{
	auto IsEnabled = BuildSettings.GetOrSet( SECTION, "bIsEnabled", "1" );
	TimestampFormat = BuildSettings.GetOrSet( SECTION, "TiemstampFormat", "{:%Y%m%d-%H:%M}" );
	ZipFileNameFormat = BuildSettings.GetOrSet( 
		SECTION, 
		"ZipFileNameFormat", "{ProjectName}_{Platform}_{ClientConfig}_{Timestamp}.zip" 
	);
	BuildFolderName = BuildSettings.GetOrSet( SECTION, "BuildFolderName", "Windows" );
	return IsEnabled == "1";
}

void ZipBuildTask::Run( BuildSettings& BuildSettings )
{
	printf( "Zipping build directory...\n" );

	FilePath ArchiveDirectoryPath = BuildSettings.GetArchiveDirectoryPath();
	String DriveName = ArchiveDirectoryPath.root_name().string();

	auto CurrentDate = std::chrono::system_clock::now();

	String FormattedTimestamp = std::vformat(
		TimestampFormat,
		std::make_format_args( CurrentDate )
	);
	String FormattedZipFileName = fmt::format(
		fmt::runtime( ZipFileNameFormat ),
		fmt::arg( "ProjectName", BuildSettings.GetProjectName() ),
		fmt::arg( "Platform", BuildSettings.GetPlatform() ),
		fmt::arg( "ClientConfig", BuildSettings.GetBuildSetting( "ClientConfig" ) ),
		fmt::arg( "Timestamp", FormattedTimestamp )
	);
	String NewFolderName = FilePath( FormattedZipFileName ).replace_extension().string();

	String Command = DriveName + " && "
		+ "cd " + ArchiveDirectoryPath.string() + " && "
		// Rename build folder to zip name (without extension)
		+ "ren " + BuildFolderName + " " + NewFolderName + " && "
		// Zip build folder
		+ "tar -c -f \"" + FormattedZipFileName + "\" " + NewFolderName;

	int Status = system( Command.c_str() );
	printf( "Zipping command finished with status: %d.\n", Status );
}

TaskRunTime ZipBuildTask::GetRunTime() const
{
	return TaskRunTime::PostBuild;
}
