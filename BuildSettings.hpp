#pragma once

#include <filesystem>

using String = std::string;
using FilePath = std::filesystem::path;

struct BuildSettings
{
	String ProjectName;
	String Platform;

	FilePath UnrealEngineDirectoryPath;
	FilePath ProjectDirectoryPath;
	FilePath ArchiveDirectoryPath;

	const FilePath UnrealAutomationToolRelativePath = "Engine/Build/BatchFiles/RunUAT.bat";
	// TODO: Check if Win64 shouldn't be replaced by our Platform variable
	const FilePath UnrealCommandExecutableRelativePath = "Engine/Binaries/Win64/UnrealEditor-Cmd.exe";
	const FilePath GameConfigRelativePath = "Config/DefaultGame.ini";

	FilePath GetUnrealAutomationToolPath() const
	{
		return UnrealEngineDirectoryPath / UnrealAutomationToolRelativePath;
	}
	FilePath GetUnrealCommandExecutablePath() const
	{
		return UnrealEngineDirectoryPath / UnrealCommandExecutableRelativePath;
	}
	FilePath GetUProjectPath() const
	{
		return ( ProjectDirectoryPath / ProjectName ).replace_extension( ".uproject" );
	}
	FilePath GetGameConfigPath() const
	{
		return ProjectDirectoryPath / GameConfigRelativePath;
	}
};