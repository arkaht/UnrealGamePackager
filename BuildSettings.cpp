#include "BuildSettings.hpp"

String BuildSettings::Get( const String& Section, const String& Key )
{
	return Ini[Section][Key];
}

String BuildSettings::GetOrSet( const String& Section, const String& Key, const String& DefaultValue )
{
	if ( !Ini[Section].has( Key ) )
	{
		Ini[Section][Key] = DefaultValue;
		bShouldSaveIni = true;
		return DefaultValue;
	}

	return Get( Section, Key );
}

String BuildSettings::GetBuildSetting( const String& Key )
{
	return Ini["BuildSettings"][Key];
}

String BuildSettings::GetProjectName()
{
	return GetBuildSetting( "ProjectName" );
}

String BuildSettings::GetPlatform()
{
	return GetBuildSetting( "Platform" );
}

FilePath BuildSettings::GetUnrealAutomationToolPath()
{
	FilePath UnrealEngineDirectoryPath = GetBuildSetting( "UnrealEngineDirectoryPath" );
	return UnrealEngineDirectoryPath / UnrealAutomationToolRelativePath;
}

FilePath BuildSettings::GetUnrealCommandExecutablePath()
{
	FilePath UnrealEngineDirectoryPath = GetBuildSetting( "UnrealEngineDirectoryPath" );
	return UnrealEngineDirectoryPath / UnrealCommandExecutableRelativePath;
}

FilePath BuildSettings::GetArchiveDirectoryPath()
{
	return GetBuildSetting( "ArchiveDirectoryPath" );
}

FilePath BuildSettings::GetProjectDirectoryPath()
{
	return GetBuildSetting( "ProjectDirectoryPath" );
}

FilePath BuildSettings::GetUProjectPath()
{
	return ( GetProjectDirectoryPath() / GetProjectName() ).replace_extension( ".uproject" );
}

FilePath BuildSettings::GetGameConfigPath()
{
	return GetProjectDirectoryPath() / GameConfigRelativePath;
}

bool BuildSettings::ShouldSaveIni() const
{
	return bShouldSaveIni;
}
