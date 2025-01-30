#include "ProjectVersionUpdateTask.hpp"

#include "fmt/format.h"
#include "fmt/args.h"
#include "fmt/color.h"

#include <regex>
#include <chrono>

const String SECTION = "TaskSettings.ProjectVersionUpdateSettings";

bool ProjectVersionUpdateTask::Initialize( BuildSettings& BuildSettings )
{
	auto IsEnabled = BuildSettings.GetOrSet( SECTION, "bIsEnabled", "1" );
	TimestampFormat = BuildSettings.GetOrSet( SECTION, "TimestampFormat", "{:%Y%m%d}" );
	ProjectVersionFormat = BuildSettings.GetOrSet( 
		SECTION, 
		"ProjectVersionFormat", "{Prefix}{Major}.{Minor}.{Timestamp}"
	);
	return IsEnabled == "1";
}

void ProjectVersionUpdateTask::Run( BuildSettings& BuildSettings )
{
	// Initialize ini structure
	CSimpleIniA GameIni {};
	GameIni.SetSpaces( false );
	GameIni.SetMultiKey( true );

	// Parse DefaultGame.ini file
	String GameConfigPath = BuildSettings.GetGameConfigPath().string();
	auto ErrorStatus = GameIni.LoadFile( GameConfigPath.c_str() );
	if ( ErrorStatus < 0 )
	{
		// TODO: Uniformize log messages
		printf( "GamePackager: Failed to read file '%s'!\n", GameConfigPath.c_str() );
		return;
	}

	// Retrieve project version
	String ProjectVersion = GameIni.GetValue( 
		"/Script/EngineSettings.GeneralProjectSettings", 
		"ProjectVersion"
	);

	// Split version
	const std::regex REGEX_VERSION( "(\\w+)(\\d+).(\\d+).(\\d+)" );
	std::smatch ProjectVersionMatch;
	std::regex_search( ProjectVersion, ProjectVersionMatch, REGEX_VERSION );

	const String ProjectVersionPrefix = ProjectVersionMatch[1];
	const String ProjectMajorVersion = ProjectVersionMatch[2];
	const String ProjectMinorVersion = ProjectVersionMatch[3];
	[[maybe_unused]] const String ProjectPatchVersion = ProjectVersionMatch[4];

	// Get current timestamp
	auto CurrentDate = std::chrono::system_clock::now();

	// Construct new project version
	const String FormattedTimestamp = std::vformat(
		TimestampFormat,
		std::make_format_args( CurrentDate )
	);
	ProjectVersion = fmt::format(
		fmt::runtime( ProjectVersionFormat ),
		fmt::arg( "Prefix", ProjectVersionPrefix ),
		fmt::arg( "Major", ProjectMajorVersion ),
		fmt::arg( "Minor", ProjectMinorVersion ),
		fmt::arg( "Patch", ProjectMinorVersion ),
		fmt::arg( "Timestamp", FormattedTimestamp )
	);

	// Override changes
	GameIni.SetValue(
		"/Script/EngineSettings.GeneralProjectSettings",
		"ProjectVersion",
		ProjectVersion.c_str(),
		nullptr,
		// NOTE: We must set bForceReplace to true to override the key instead of adding it
		//		 as the multikey option is enabled.
		/* bForceReplace */ true
	);

	const SI_Error SaveStatus = GameIni.SaveFile( GameConfigPath.c_str(), true );
	if ( SaveStatus < 0 )
	{
		switch ( SaveStatus )
		{
			case SI_FAIL:
				fmt::print(
					fmt::fg( fmt::color::red ),
					"GamePackager.ProjectVersionUpdateTask: Failed to set 'ProjectVersion={0}': generic failure.\n",
					ProjectVersion
				);
				break;
			case SI_NOMEM:
				fmt::print(
					fmt::fg( fmt::color::red ),
					"GamePackager.ProjectVersionUpdateTask: Failed to set 'ProjectVersion={0}': out-of-memory.\n",
					ProjectVersion
				);
				break;
			case SI_FILE:
				fmt::print(
					fmt::fg( fmt::color::red ),
					"GamePackager.ProjectVersionUpdateTask: Failed to set 'ProjectVersion={0}': file error.\n",
					ProjectVersion
				);
				break;
		}
	}
	else
	{
		fmt::print( "GamePackager.ProjectVersionUpdateTask: Set 'ProjectVersion={0}'.\n", ProjectVersion );
	}
}

TaskRunTime ProjectVersionUpdateTask::GetRunTime() const
{
	return TaskRunTime::PreBuild;
}
