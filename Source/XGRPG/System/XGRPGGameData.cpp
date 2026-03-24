#include "XGRPGGameData.h"

#include "XGRPGAssetManager.h"

UXGRPGGameData::UXGRPGGameData()
{
}

const UXGRPGGameData& UXGRPGGameData::Get()
{
	return UXGRPGAssetManager::Get().GetGameData();
}
