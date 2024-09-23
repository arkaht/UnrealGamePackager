#include "BuildSettings.hpp"

BuildSettings::BuildSettings( const std::string& FileName )
	: FileName( FileName )
{
	Ini.SetSpaces( false );

	// Load settings from file
	auto ErrorStatus = Ini.LoadFile( FileName.c_str() );
	if ( ErrorStatus == SI_OK ) return;
	if ( ErrorStatus != SI_FILE )
	{
		printf(
			"GamePackager: Error when loading the build settings, error status! %d\n",
			ErrorStatus
		);
		return;
	}

	// Construct default settings
	// TODO: Find a uniform and convenient way to declare default values
	Ini.SetValue( "BuildSettings", "ProjectName", "ProjectName" );
	Ini.SetValue( "BuildSettings", "UnrealEngineDirectoryPath", "C:/" );
	Ini.SetValue( "BuildSettings", "ArchiveDirectoryPath", "C:/" );
	Ini.SetValue( "BuildSettings", "ProjectDirectoryPath", "C:/" );
	Ini.SetValue( "BuildSettings", "Platform", "Win64" );
	Ini.SetValue( "BuildSettings", "ClientConfig", "Development" );
	Ini.SetValue( "BuildSettings", "CookedMaps", "" );

	// Generate settings file
	Ini.SaveFile( FileName.c_str() );

	// Print info to user
	FilePath ExecutableDirectoryPath = std::filesystem::current_path();
	FilePath SettingsPath = ExecutableDirectoryPath / FileName;
	printf( "GamePackager: Created a default settings file at '%s'\n", SettingsPath.string().c_str() );
}

String BuildSettings::Get( const String& Section, const String& Key ) const
{
	const char* Value = Ini.GetValue( Section.c_str(), Key.c_str() );
	if ( Value == nullptr ) return "";
	return Value;
}

String BuildSettings::GetOrSet( const String& Section, const String& Key, const String& DefaultValue )
{
	auto Value = Get( Section.c_str(), Key.c_str() );
	if ( Value.empty() )
	{
		Ini.SetValue( Section.c_str(), Key.c_str(), DefaultValue.c_str() );
		bShouldSaveIni = true;
		return DefaultValue;
	}

	return Get( Section, Key );
}

String BuildSettings::GetBuildSetting( const String& Key ) const
{
	return Get( "BuildSettings", Key );
}

String BuildSettings::GetProjectName() const
{
	return GetBuildSetting( "ProjectName" );
}

String BuildSettings::GetPlatform() const
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

FilePath BuildSettings::GetArchiveDirectoryPath() const
{
	return GetBuildSetting( "ArchiveDirectoryPath" );
}

FilePath BuildSettings::GetProjectDirectoryPath() const
{
	return GetBuildSetting( "ProjectDirectoryPath" );
}

FilePath BuildSettings::GetUProjectPath() const
{
	return ( GetProjectDirectoryPath() / GetProjectName() ).replace_extension( ".uproject" );
}

FilePath BuildSettings::GetGameConfigPath() const
{
	return GetProjectDirectoryPath() / GameConfigRelativePath;
}

void BuildSettings::SaveIfNeeded()
{
	if ( !bShouldSaveIni ) return;

	Ini.SaveFile( FileName.c_str() );
	printf( "GamePackager: Saved the settings file to add new values\n" );
}
