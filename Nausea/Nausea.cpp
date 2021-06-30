// Copyright 2020-2021 Jean-David Veilleux-Foppiano. All Rights Reserved.


#include "Nausea.h"
#include "Modules/ModuleManager.h"

#if WITH_EDITOR
#include "DetailCustomizations.h"
#include "PropertyEditorModule.h"
#include "System/PropertyDetails/NauseaDetails.h"
#endif


#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebugger.h"
#include "System/DebugCategory/GameplayDebuggerCategory_Mission.h"
#include "System/DebugCategory/GameplayDebuggerCategory_Routine.h"
#endif // WITH_GAMEPLAY_DEBUGGER


//LOG CATEGORIES
DEFINE_LOG_CATEGORY(LogInventoryManager);
DEFINE_LOG_CATEGORY(LogWeapon);
DEFINE_LOG_CATEGORY(LogFireMode);
DEFINE_LOG_CATEGORY(LogAmmo);

class FNauseaModule : public FDefaultGameModuleImpl
{
	// Begin IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End IModuleInterface
};

IMPLEMENT_PRIMARY_GAME_MODULE(FNauseaModule, Nausea, "Nausea");

void FNauseaModule::StartupModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
	GameplayDebuggerModule.RegisterCategory("Mission", IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_Mission::MakeInstance), EGameplayDebuggerCategoryState::EnabledInGameAndSimulate, 5);
	GameplayDebuggerModule.RegisterCategory("Routine", IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_Routine::MakeInstance), EGameplayDebuggerCategoryState::EnabledInGameAndSimulate, 6);
	GameplayDebuggerModule.NotifyCategoriesChanged();
#endif

#if WITH_EDITOR
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Get the editor to load all of the basic property types so that we can append to all the class name map entries.
	FDetailCustomizationsModule& DetailCustomizationsModule = FModuleManager::LoadModuleChecked<FDetailCustomizationsModule>("DetailCustomizations");

	FOnGetDetailCustomizationInstance NauseaDetailsInstance = FOnGetDetailCustomizationInstance::CreateStatic(&FNauseaDetails::MakeInstance);

	const FCustomDetailLayoutNameMap& CustomDetailLayoutNameMap = PropertyModule.GetClassNameToDetailLayoutNameMap();
	TArray<FName> KeyList;
	CustomDetailLayoutNameMap.GenerateKeyArray(KeyList);
	for (const FName& Key : KeyList)
	{
		PropertyModule.RegisterCustomClassLayout(Key, NauseaDetailsInstance);
	}

	PropertyModule.NotifyCustomizationModuleChanged();
#endif
}

void FNauseaModule::ShutdownModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.UnregisterCategory("Mission");
		GameplayDebuggerModule.UnregisterCategory("Routine");
		GameplayDebuggerModule.NotifyCategoriesChanged();
	}
#endif

#if WITH_EDITOR
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout("Object");
	PropertyModule.NotifyCustomizationModuleChanged();
#endif
}