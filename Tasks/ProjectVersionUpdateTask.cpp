#include "ProjectVersionUpdateTask.hpp"

#include <regex>
#include <chrono>

const std::string SECTION = "TaskSettings.ProjectVersionUpdateSettings";

bool ProjectVersionUpdateTask::Initialize( BuildSettings& BuildSettings )
{
	auto IsEnabled = BuildSettings.GetOrSet( SECTION, "bIsEnabled", "1" );
	return IsEnabled == "1";
}

void ProjectVersionUpdateTask::Run( BuildSettings& BuildSettings )
{
	// Parse DefaultGame.ini file
    String GameConfigPath = BuildSettings.GetGameConfigPath().string();

	CSimpleIniA GameIni;
	GameIni.SetSpaces( false );
	GameIni.SetMultiKey( true );

	auto ErrorStatus = GameIni.LoadFile( GameConfigPath.c_str() );
	if ( ErrorStatus < 0 )
	{
		// TODO: Uniformize log messages
		printf( "GamePackager: Failed to read file '%s'!\n", GameConfigPath.c_str() );
		return;
	}

	// Retrieve project version
	std::string ProjectVersion = GameIni.GetValue( 
		"/Script/EngineSettings.GeneralProjectSettings", 
		"ProjectVersion"
	);

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
	GameIni.SetValue(
		"/Script/EngineSettings.GeneralProjectSettings",
		"ProjectVersion",
		ProjectVersion.c_str()
	);
	GameIni.SaveFile( GameConfigPath.c_str(), true );

	printf("");
}

TaskRunTime ProjectVersionUpdateTask::GetRunTime() const
{
    return TaskRunTime::PreBuild;
}
