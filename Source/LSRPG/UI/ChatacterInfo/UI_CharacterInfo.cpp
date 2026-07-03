#include "UI_CharacterInfo.h"


#include "LSRPGAbilitySystemComponent.h"
#include "LSRPGCharacterBase.h"
#include "LSRPGCharacterAttributeSet.h"

#include "Components\TextBlock.h"

void UUI_CharacterInfo::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_CharacterInfo::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_CharacterInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ALSRPGCharacterBase* InCharacter = UI_GetLSRPGCharacterBase())
	{

		if (ULSRPGAbilitySystemComponent* InASC = InCharacter->GetLSRPGAbilitySystemComponent())
		{

			if (const ULSRPGCharacterAttributeSet* InAttribute =
			Cast<ULSRPGCharacterAttributeSet>(InASC->GetAttributeSet(ULSRPGCharacterAttributeSet::StaticClass())))
			{
				if (Health)
				{

					float  HealthNum = InAttribute->GetHealth();
					float  MaxHealthNum = InAttribute->GetMaxHealth();

					FString HealthString = FString::Printf(TEXT("%s"), *FString::SanitizeFloat(HealthNum));
					FString MaxHealthString = FString::Printf(TEXT("%s"), *FString::SanitizeFloat(MaxHealthNum));

					Health->SetText(FText::FromString(HealthString + "/" + MaxHealthString));
				}


				if (Mana)
				{
					//太奢侈了,产生很多匿名对象的拷贝
					float  ManaNum = InAttribute->GetMana();
					float  MaxManaNum = InAttribute->GetMaxMana();
					FString ManaString = FString::Printf(TEXT("%s"), *FString::SanitizeFloat(ManaNum));
					FString MaxManaString = FString::Printf(TEXT("%s"), *FString::SanitizeFloat(MaxManaNum));

					Mana->SetText(FText::FromString(ManaString + "/" + MaxManaString));
				}

				if (Stamina)
				{
					//太奢侈了,产生很多匿名对象的拷贝
					float  StaminaNum = InAttribute->GetStamina();
					float  MaxStaminaNum = InAttribute->GetMaxStamina();
					FString StaminaString = FString::Printf(TEXT("%s"), *FString::SanitizeFloat(StaminaNum));
					FString MaxStaminaString = FString::Printf(TEXT("%s"), *FString::SanitizeFloat(MaxStaminaNum));

					Stamina->SetText(FText::FromString(StaminaString + "/" + MaxStaminaString));
				}

				if (Attack)
				{
					//太奢侈了,产生很多匿名对象的拷贝
					float  AttackNum = InAttribute->GetAttack();

					FString AttackString = FString::Printf(TEXT("%s"), *FString::SanitizeFloat(AttackNum));

					Attack->SetText(FText::FromString(AttackString));
				}


				if (Defense)
				{
					//太奢侈了,产生很多匿名对象的拷贝
					float  DefenseNum = InAttribute->GetDefense();

					FString DefenseString = FString::Printf(TEXT("%s"), *FString::SanitizeFloat(DefenseNum));

					Defense->SetText(FText::FromString(DefenseString));
				}

				if (Exp)
				{
					//太奢侈了,产生很多匿名对象的拷贝
					float  ExpNum = InAttribute->GetExp();
					float  MaxExpNum = InAttribute->GetMaxExp();
					FString  ExpString = FString::Printf(TEXT("%s"), *FString::SanitizeFloat(ExpNum));
					FString MaxExpString = FString::Printf(TEXT("%s"), *FString::SanitizeFloat(MaxExpNum));

					Exp->SetText(FText::FromString(ExpString + "/" + MaxExpString));

				}





			}
		}
	}




}
