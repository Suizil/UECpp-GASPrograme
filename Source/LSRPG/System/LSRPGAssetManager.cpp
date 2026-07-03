#include "LSRPGAssetManager.h"

#include "LSRPGLogChannels.h"

#include"LSRPGItem.h"


#include "AbilitySystemGlobals.h"

#include "LSRPGGameData.h"

const FPrimaryAssetType	ULSRPGAssetManager::PotionItemType = TEXT("Potion");
const FPrimaryAssetType	ULSRPGAssetManager::TokenItemType = TEXT("Token");
const FPrimaryAssetType	ULSRPGAssetManager::EquipmentItemType = TEXT("Equipment");


ULSRPGAssetManager::ULSRPGAssetManager()
:Super()
{
}

void ULSRPGAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	//TODO
	UAbilitySystemGlobals::Get().InitGlobalData();

}

ULSRPGAssetManager& ULSRPGAssetManager::Get()
{
	check(GEngine);

	if (ULSRPGAssetManager* Singleton = Cast<ULSRPGAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}


	UE_LOG(LogLSRPG, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to LyraAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<ULSRPGAssetManager>();


}

const ULSRPGGameData& ULSRPGAssetManager::GetGameData()
{

	return GetOrLoadTypedGameData<ULSRPGGameData>(LSRPGGameDataPath);
}

ULSRPGItem* ULSRPGAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	ULSRPGItem* LoadedItem = Cast<ULSRPGItem>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}

UPrimaryDataAsset* ULSRPGAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);

	if (!DataClassPath.IsNull())
	{

#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(NSLOCTEXT("LSRPGEditor", "BeginLoadingGameDataTask", "Loading GameData {0}"), FText::FromName(DataClass->GetFName())));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif

		UE_LOG(LogLSRPG, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{

			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}


		}


	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		UE_LOG(LogLSRPG, Fatal, TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s."), *DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
	}

	return Asset;
}

UObject* ULSRPGAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool ULSRPGAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void ULSRPGAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}
