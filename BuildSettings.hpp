#pragma once

#include <filesystem>

#include "mini/ini.h"

using String = std::string;
using FilePath = std::filesystem::path;

class BuildSettings
{
public:
	BuildSettings( const mINI::INIStructure& Ini )
		: Ini( Ini )
	{}

	String Get( const String& Section, const String& Key );
	String GetOrSet( const String& Section, const String& Key, const String& DefaultValue );
	String GetBuildSetting( const String& Key );

	String GetProjectName();
	String GetPlatform();

	FilePath GetUnrealAutomationToolPath();
	FilePath GetUnrealCommandExecutablePath();
	FilePath GetArchiveDirectoryPath();
	FilePath GetProjectDirectoryPath();
	FilePath GetUProjectPath();
	FilePath GetGameConfigPath();

	bool ShouldSaveIni() const;

public:
	const FilePath UnrealAutomationToolRelativePath = "Engine/Build/BatchFiles/RunUAT.bat";
	// TODO: Check if Win64 shouldn't be replaced by our Platform variable
	const FilePath UnrealCommandExecutableRelativePath = "Engine/Binaries/Win64/UnrealEditor-Cmd.exe";
	const FilePath GameConfigRelativePath = "Config/DefaultGame.ini";

public:
	mINI::INIStructure Ini {};
	bool bShouldSaveIni = false;
};