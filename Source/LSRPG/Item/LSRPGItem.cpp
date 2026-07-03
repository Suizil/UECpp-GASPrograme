#include "LSRPGItem.h"

bool ULSRPGItem::IsConsumable() const
{
	if (MaxCount <= 0)
	{
		return false;
	}
	return true;
}

FString ULSRPGItem::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}

FPrimaryAssetId ULSRPGItem::GetPrimaryAssetId() const
{
	// This is a DataAsset and not a blueprint so we can just use the raw FName
	// For blueprints you need to handle stripping the _C suffix
	return FPrimaryAssetId(ItemType, GetFName());
}
