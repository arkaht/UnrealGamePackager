#include <iostream>
#include <string>
#include <filesystem>
#include <array>

#include "BuildSettings.hpp"
#include "Tasks/ZipBuildTask.h"
#include "Tasks/ProjectVersionUpdateTask.h"

template<typename T>
using UniquePtr = std::unique_ptr<T>;
template<typename T>
using Vector = std::vector<T>;

BuildSettings ConstructBuildSettings()
{
	String SettingsFileName = "GamePackagerSettings.ini";
	mINI::INIFile SettingsFile( SettingsFileName );

	mINI::INIStructure SettingsIni {};
	if ( !SettingsFile.read( SettingsIni ) )
	{
		// Construct default build settings
		auto& BuildSettingsSection = SettingsIni["BuildSettings"];
		BuildSettingsSection["ProjectName"] = "ProjectName";
		BuildSettingsSection["UnrealEngineDirectoryPath"] = "C:/";
		BuildSettingsSection["ArchiveDirectoryPath"] = "C:/";
		BuildSettingsSection["ProjectDirectoryPath"] = "C:/";
		BuildSettingsSection["Platform"] = "Win64";
		BuildSettingsSection["CookedMaps"] = "";

		// Generate settings file
		SettingsFile.generate( SettingsIni, false );

		FilePath ExecutableDirectoryPath = std::filesystem::current_path();
		FilePath SettingsPath = ExecutableDirectoryPath / SettingsFileName;
		printf( "GamePackager: Created a default settings file at '%s'\n", SettingsPath.string().c_str() );
	}

	// Construct build settings out of the structure
	BuildSettings BuildSettings {};
	BuildSettings.ProjectName = SettingsIni["BuildSettings"]["ProjectName"];
	BuildSettings.UnrealEngineDirectoryPath = SettingsIni["BuildSettings"]["UnrealEngineDirectoryPath"];
	BuildSettings.ArchiveDirectoryPath = SettingsIni["BuildSettings"]["ArchiveDirectoryPath"];
	BuildSettings.ProjectDirectoryPath = SettingsIni["BuildSettings"]["ProjectDirectoryPath"];
	BuildSettings.Platform = SettingsIni["BuildSettings"]["Platform"];
	BuildSettings.Ini = SettingsIni;

	return BuildSettings;
}

int main()
{
	// Create all the automation commands
	Vector<UniquePtr<Task>> Tasks {};
	Tasks.push_back( std::make_unique<ProjectVersionUpdateTask>() );
	Tasks.push_back( std::make_unique<ZipBuildTask>() );

	BuildSettings BuildSettings = ConstructBuildSettings();

	// Initialize the automation commands
	for ( auto& Task : Tasks )
	{
		Task->Initialize( BuildSettings );
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
		"-target=" + BuildSettings.ProjectName,
		"-unrealexe=\"" + BuildSettings.GetUnrealCommandExecutablePath().string() + "\"",
		"-platform=" + BuildSettings.Platform,
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
		"-archivedirectory=\"" + BuildSettings.ArchiveDirectoryPath.string() + "\"",
		"-clientconfig=Development",
		// TODO: Add map-content cooking only
		//"-map=?+?",
	};

	// Add map-content cooking only
	std::string CookedMaps = BuildSettings.Ini["BuildSettings"]["CookedMaps"];
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

	bool bShouldBuild = false;
	printf( "GamePackager: Do you want to build? " );
	std::cin >> bShouldBuild;
	if ( bShouldBuild )
	{
		// Run the PreBuild automation commands
		for ( auto& Task : Tasks )
		{
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
			if ( Task->GetRunTime() != TaskRunTime::PostBuild ) continue;

			Task->Run( BuildSettings );
		}
	}

	printf( "GamePackager: Should Exit? " );
	int Temp = 0;
	std::cin >> Temp;
}