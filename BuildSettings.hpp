#pragma once

#include <filesystem>

#include "simpleini/SimpleIni.h"

using String = std::string;
using FilePath = std::filesystem::path;

class BuildSettings
{
public:
	BuildSettings( const std::string& FileName );

	String Get( const String& Section, const String& Key ) const;
	String GetOrSet( const String& Section, const String& Key, const String& DefaultValue );
	String GetBuildSetting( const String& Key ) const;

	String GetProjectName() const;
	String GetPlatform() const;

	FilePath GetUnrealAutomationToolPath();
	FilePath GetUnrealCommandExecutablePath();
	FilePath GetArchiveDirectoryPath() const;
	FilePath GetProjectDirectoryPath() const;
	FilePath GetUProjectPath() const;
	FilePath GetGameConfigPath() const;

	void SaveIfNeeded();

public:
	const FilePath UnrealAutomationToolRelativePath = "Engine/Build/BatchFiles/RunUAT.bat";
	// TODO: Check if Win64 shouldn't be replaced by our Platform variable
	const FilePath UnrealCommandExecutableRelativePath = "Engine/Binaries/Win64/UnrealEditor-Cmd.exe";
	const FilePath GameConfigRelativePath = "Config/DefaultGame.ini";

public:
	CSimpleIniA Ini {};
	std::string FileName {};
	bool bShouldSaveIni = false;
};