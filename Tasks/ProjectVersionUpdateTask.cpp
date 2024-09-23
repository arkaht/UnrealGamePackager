#include "ProjectVersionUpdateTask.h"

#include <regex>
#include <chrono>

#include "mini/ini.h"

void ProjectVersionUpdateTask::Initialize( const BuildSettings& BuildSettings )
{
}

void ProjectVersionUpdateTask::Run( const BuildSettings& BuildSettings )
{
	// Parse DefaultGame.ini file
    String GameConfigPath = BuildSettings.GetGameConfigPath().string();
	mINI::INIFile GameIniFile( GameConfigPath );
	mINI::INIStructure GameIniData {};
	if ( !GameIniFile.read( GameIniData ) )
	{
		printf( "Failed to read file '%s'!\n", GameConfigPath.c_str() );
		return;
	}

	// Retrieve project version
	std::string& ProjectVersion = GameIniData["/Script/EngineSettings.GeneralProjectSettings"]["ProjectVersion"];

	// Split version
	const std::regex REGEX_VERSION( "(\\w+)(\\d+).(\\d+).(\\d+)" );
	std::smatch ProjectVersionMatch;
	std::regex_search( ProjectVersion, ProjectVersionMatch, REGEX_VERSION );

	std::string ProjectVersionPrefix = ProjectVersionMatch[1];
	std::string ProjectMajorVersion = ProjectVersionMatch[2];
	std::string ProjectMinorVersion = ProjectVersionMatch[3];
	[[maybe_unused]] std::string ProjectPatchVersion = ProjectVersionMatch[4];

	// Get current timestamp
	auto CurrentDate = std::chrono::system_clock::now();

	// Construct new project version
	ProjectVersion = std::format(
		"{0}{1}.{2}.{3}", 
		ProjectVersionPrefix,
		ProjectMajorVersion,
		ProjectMinorVersion,
		std::format( "{:%Y%m%d}", CurrentDate )
	);

	// Override changes
	GameIniFile.write( GameIniData, true );
}

TaskRunTime ProjectVersionUpdateTask::GetRunTime() const
{
    return TaskRunTime::PreBuild;
}
