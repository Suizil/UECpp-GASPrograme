#include "XGRPGEquipment.h"

#include "XGRPGAssetManager.h"

UXGRPGEquipment::UXGRPGEquipment()
{
	ItemType = UXGRPGAssetManager::EquipmentItemType;
	MaxCount = 1;
}