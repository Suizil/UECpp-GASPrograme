#include "LSRPGGameData.h"

#include "LSRPGAssetManager.h"

ULSRPGGameData::ULSRPGGameData()
{
}

const ULSRPGGameData& ULSRPGGameData::Get()
{
	return ULSRPGAssetManager::Get().GetGameData();
}
