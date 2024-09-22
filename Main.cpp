#include <iostream>
#include <string>
#include <filesystem>
#include <array>

#include "BuildSettings.hpp"
#include "AutomationCommands/ZipBuildAutomationCommand.h"
#include "AutomationCommands/ProjectVersionUpdateAutomationCommand.h"

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
		BuildSettingsSection["ArchiveDirectoryPath"] = "";
		BuildSettingsSection["ProjectDirectoryPath"] = "";
		BuildSettingsSection["Platform"] = "Win64";

		// Generate settings file
		SettingsFile.generate( SettingsIni, false );

		FilePath ExecutableDirectoryPath = std::filesystem::current_path();
		FilePath SettingsPath = ExecutableDirectoryPath / SettingsFileName;
		printf( "GamePackager: Created a default settings file at '%s'\n", SettingsPath.string().c_str() );
	}

	// Construct build settings out of the structure
	BuildSettings BuildSettings {};
	BuildSettings.ProjectName = SettingsIni["BuildSettings"]["ProjectName"];
	BuildSettings.UnrealEngineDirectoryPath = "C:/Program Files/Epic Games/UE_5.4/";
	BuildSettings.ArchiveDirectoryPath = "C:/Users/arthu/OneDrive/Documents/Unreal Projects/MetroXHunter/Build";
	BuildSettings.ProjectDirectoryPath = "C:/Users/arthu/OneDrive/Documents/Unreal Projects/MetroXHunter/";
	BuildSettings.Platform = SettingsIni["BuildSettings"]["Platform"];

	return BuildSettings;
}

int main()
{
	// Create all the automation commands
	Vector<UniquePtr<AutomationCommand>> AutomationCommands {};
	AutomationCommands.push_back( std::make_unique<ProjectVersionUpdateAutomationCommand>() );
	AutomationCommands.push_back( std::make_unique<ZipBuildAutomationCommand>() );

	BuildSettings BuildSettings = ConstructBuildSettings();

	// Initialize the automation commands
	for ( auto& AutomationCommand : AutomationCommands )
	{
		AutomationCommand->Initialize( BuildSettings );
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

	std::string CommandLine = "";
	for ( std::string Argument : CommandArguments )
	{
		CommandLine += Argument + " ";
	}

	printf( "Command Line: %s\n", CommandLine.c_str() );

	bool bShouldBuild = false;
	printf( "Do you want to build? " );
	std::cin >> bShouldBuild;
	if ( bShouldBuild )
	{
		// Run the PreBuild automation commands
		for ( auto& AutomationCommand : AutomationCommands )
		{
			if ( AutomationCommand->GetRunTime() != AutomationCommandRunTime::PreBuild ) continue;

			AutomationCommand->Run( BuildSettings );
		}

		// Run build command
		printf( "Running build command...\n" );
		int Status = system( CommandLine.c_str() );
		printf( "Build command finished with status: %d.\n", Status );

		// Run the PostBuild automation commands
		for ( auto& AutomationCommand : AutomationCommands )
		{
			if ( AutomationCommand->GetRunTime() != AutomationCommandRunTime::PostBuild ) continue;

			AutomationCommand->Run( BuildSettings );
		}
	}

	printf( "Exit? " );
	int Temp = 0;
	std::cin >> Temp;
}