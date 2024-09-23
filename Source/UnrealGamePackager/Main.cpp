#include <iostream>
#include <string>
#include <filesystem>
#include <array>

#include "UnrealGamePackager/BuildSettings.hpp"
#include "UnrealGamePackager/Tasks/ZipBuildTask.hpp"
#include "UnrealGamePackager/Tasks/ProjectVersionUpdateTask.hpp"

template<typename T>
using UniquePtr = std::unique_ptr<T>;
template<typename T>
using Vector = std::vector<T>;

constexpr auto SETTINGS_FILE_NAME = "GamePackagerSettings.ini";

int main()
{
	// Create all the automation commands
	Vector<UniquePtr<Task>> Tasks {};
	Tasks.push_back( std::make_unique<ProjectVersionUpdateTask>() );
	Tasks.push_back( std::make_unique<ZipBuildTask>() );

	BuildSettings BuildSettings( SETTINGS_FILE_NAME );

	// Initialize the automation commands
	for ( auto& Task : Tasks )
	{
		Task->bCanRun = Task->Initialize( BuildSettings );
	}

	std::vector<std::string> CommandArguments {
		"C: && ",
		"\"" + BuildSettings.GetUnrealAutomationToolPath().string() + "\"",
		"BuildCookRun",
		"-nop4", 
		"-utf8output",
		"-nocompileeditor",
		"-skipbuildeditor",
		"-cook",
		"-project=\"" + BuildSettings.GetUProjectPath().string() + "\"",
		"-target=" + BuildSettings.GetProjectName(),
		"-unrealexe=\"" + BuildSettings.GetUnrealCommandExecutablePath().string() + "\"",
		"-platform=" + BuildSettings.GetPlatform(),
		"-installed",
		"-stage",
		"-archive",
		"-package",
		"-build",
		"-pak",
		"-iostore",
		"-compressed",
		"-prereqs",
		"-applocaldirectory=$(EngineDir)/Binaries/ThirdParty/AppLocalDependencies",
		"-archivedirectory=\"" + BuildSettings.GetArchiveDirectoryPath().string() + "\"",
		"-clientconfig=" + BuildSettings.GetBuildSetting( "ClientConfig" ),
	};

	// Add map-content cooking only
	std::string CookedMaps = BuildSettings.GetBuildSetting( "CookedMaps" );
	if ( !CookedMaps.empty() )
	{
		CommandArguments.push_back( "-map=" + CookedMaps );
		printf( "GamePackager: Added map cooking command argument\n" );
	}

	// Constructing command line
	std::string CommandLine = "";
	for ( std::string Argument : CommandArguments )
	{
		CommandLine += Argument + " ";
	}

	printf( "GamePackager: Command Line: %s\n", CommandLine.c_str() );

	bool bShouldBuild = BuildSettings.GetOrSet( "UnrealGamePackagerSettings", "bBuildEnabled", "0" ) == "1";
	if ( bShouldBuild )
	{
		// Run the PreBuild automation commands
		for ( auto& Task : Tasks )
		{
			if ( !Task->bCanRun ) continue;
			if ( Task->GetRunTime() != TaskRunTime::PreBuild ) continue;

			Task->Run( BuildSettings );
		}

		// Run build command
		printf( "GamePackager: Running build command...\n" );
		int Status = system( CommandLine.c_str() );
		printf( "GamePackager: Build command finished with status: %d.\n", Status );

		// Run the PostBuild automation commands
		for ( auto& Task : Tasks )
		{
			if ( !Task->bCanRun ) continue;
			if ( Task->GetRunTime() != TaskRunTime::PostBuild ) continue;

			Task->Run( BuildSettings );
		}
	}
	else
	{
		printf( "GamePackager: Configure your settings file '%s'.\n", SETTINGS_FILE_NAME );
		printf( "GamePackager: Set 'bBuildEnabled' to '1' to process a build.\n" );
	}
	
	BuildSettings.SaveIfNeeded();

	printf( "GamePackager: Should Exit? " );
	int Temp = 0;
	std::cin >> Temp;
}