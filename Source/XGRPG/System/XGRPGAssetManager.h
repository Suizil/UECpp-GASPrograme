
#pragma once

#include "Engine/AssetManager.h"
#include "Templates/SubclassOf.h"

#include "XGRPGAssetManager.generated.h"

class UPrimaryDataAsset;
class UXGRPGGameData;
class UXGRPGItem;
/**
 * UXGRPGAssetManager
 *
 *	Game implementation of the asset manager that overrides functionality and stores game-specific types.
 *	It is expected that most games will want to override AssetManager as it provides a good place for game-specific loading logic.
 *	This class is used by setting 'AssetManagerClassName' in DefaultEngine.ini.
 */
UCLASS(Config = Game)
class XGRPG_API UXGRPGAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	UXGRPGAssetManager();


	virtual void StartInitialLoading() override;


	/** Static types for items */
	static const FPrimaryAssetType	PotionItemType;
	static const FPrimaryAssetType	TokenItemType;
	static const FPrimaryAssetType	EquipmentItemType;

	// Returns the AssetManager singleton object.
	static UXGRPGAssetManager& Get();

	// Returns the subclass referenced by a TSoftClassPtr.  This will synchronously load the asset if it's not already loaded.
	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);


	const UXGRPGGameData& GetGameData();

	/**
	* Synchronously loads an RPGItem subclass, this can hitch but is useful when you cannot wait for an async load
	* This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	*
	* @param PrimaryAssetId The asset identifier to load
	* @param bDisplayWarning If true, this will log a warning if the item failed to load
	*/
	UXGRPGItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);

protected:


	template <typename GameDataClass>
	const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
	{

		if (TObjectPtr<UPrimaryDataAsset> const* pResult = GameDataMap.Find(GameDataClass::StaticClass()))
		{
			return *CastChecked<GameDataClass>(*pResult);

		}


		// Does a blocking load if needed
		return *CastChecked<const GameDataClass>(LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
	}

	UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType);


	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);


	static bool ShouldLogAssetLoads();

	// Thread safe way of adding a loaded asset to keep in memory.
	void AddLoadedAsset(const UObject* Asset);




protected:

	// Global game data asset to use.
	UPROPERTY(Config)
	TSoftObjectPtr<UXGRPGGameData> XGRPGGameDataPath;

	// Loaded version of the game data
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;


private:

	// Assets loaded and tracked by the asset manager.
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Used for a scope lock when modifying the list of load assets.
	FCriticalSection LoadedAssetsCritical;

};

template<typename AssetType>
TSubclassOf<AssetType> UXGRPGAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}