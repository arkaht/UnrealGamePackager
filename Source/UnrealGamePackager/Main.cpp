#include <iostream>
#include <string>
#include <filesystem>
#include <array>

#include "fmt/format.h"
#include "fmt/color.h"

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
#ifdef WIN32
	// Enable color support on the Windows console
	HANDLE Handle = GetStdHandle( STD_OUTPUT_HANDLE );
	DWORD Mode = 0;
	GetConsoleMode( Handle, &Mode );
	SetConsoleMode( Handle, Mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING );
#endif

	// Create all the tasks
	Vector<UniquePtr<Task>> Tasks {};
	Tasks.push_back( std::make_unique<ProjectVersionUpdateTask>() );
	Tasks.push_back( std::make_unique<ZipBuildTask>() );

	BuildSettings BuildSettings( SETTINGS_FILE_NAME );

	// Initialize the tasks
	for ( auto& Task : Tasks )
	{
		Task->bCanRun = Task->Initialize( BuildSettings );
	}

	// Assemble all build options
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

	bool bShouldBuild = BuildSettings.GetOrSet( 
		"UnrealGamePackagerSettings", 
		"bBuildEnabled", "0" 
	) == "1";
	if ( bShouldBuild )
	{
		// Run the PreBuild tasks
		fmt::print( fmt::fg( fmt::color::gold ), "GamePackager: Running PreBuild tasks...\n" );
		for ( auto& Task : Tasks )
		{
			if ( !Task->bCanRun ) continue;
			if ( Task->GetRunTime() != TaskRunTime::PreBuild ) continue;

			Task->Run( BuildSettings );
		}

		// Run build command
		fmt::print(
			fmt::fg( fmt::color::gold ),
			"GamePackager: Running build command...\n"
		);
		int Status = system( CommandLine.c_str() );
		fmt::print(
			fmt::fg( Status == 0 ? fmt::color::green : fmt::color::red ),
			"GamePackager: Build command finished with status: {0}.\n",
			Status
		);

		// Run the PostBuild tasks
		fmt::print( fmt::fg( fmt::color::gold ), "GamePackager: Running PostBuild tasks...\n" );
		for ( auto& Task : Tasks )
		{
			if ( !Task->bCanRun ) continue;
			if ( Task->GetRunTime() != TaskRunTime::PostBuild ) continue;

			Task->Run( BuildSettings );
		}

		fmt::print( fmt::fg( fmt::color::gold ), "GamePackager: Build process finished.\n" );
	}
	else
	{
		fmt::print(
			fmt::fg( fmt::color::gold ),
			"GamePackager: Configure your settings file '{0}'.\n",
			SETTINGS_FILE_NAME
		);
		fmt::print(
			fmt::fg( fmt::color::gold ),
			"GamePackager: Set 'bBuildEnabled' to '1' to process a build.\n"
		);
	}
	
	BuildSettings.SaveIfNeeded();

	printf( "GamePackager: Should Exit? " );
	int Temp = 0;
	std::cin >> Temp;
}