// Copyright Epic Games, Inc. All Rights Reserved.

#include "RZ_UtilityPlugin.h"

#define LOCTEXT_NAMESPACE "FRZ_UtilityPluginModule"

void FRZ_UtilityPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FRZ_UtilityPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRZ_UtilityPluginModule, RZ_UtilityPlugin)