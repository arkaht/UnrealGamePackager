# UnrealGamePackager
 
> Simple C++ CLI program to package an Unreal Engine 5 game project within a custom workflow.

## Tasks
This program comes with a simple and extensible system that allows you to add additional steps (a.k.a. "Tasks") before and after the build process. Such tasks can perform basically anything. The current implemented tasks are:
+ `ProjectVersionUpdateTask`: an update of the `ProjectVersion` variable from the `DefaultGame.ini` using the timestamp at the moment of the build while preserving the actual version (if compatible with [semantic versioning](https://semver.org))
+ `ZipBuildTask`: a rename and/or .zip archive of the build with a custom nomenclature (e.g. `MyProject_Win64_Shipping_2024-09-23_18h43.zip`)

## Settings
A `GamePackagerSettings.ini` file is automatically created for the first launch of the executable to provide the user with settings to change how the build and tasks will behave. It also helps to create a common and shareable settings file for the entire project team to use. 

> [!NOTE]
> *For now, user settings such as engine, project and archive directories paths are also stored inside that file and may be changed later.*