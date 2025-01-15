// Fill out your copyright notice in the Description page of Project Settings.


#include "Component_Inventory.h"
#include "InventoryStructs.h"

UComponent_Inventory::UComponent_Inventory()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UComponent_Inventory::BeginPlay()
{
	Super::BeginPlay();

}

void UComponent_Inventory::SetGrabCounter()
{
	if (CreatedSigils.Num() <= 0)
	{
		CurrentGrabCounter = FSigilSettings();
		return;
	}
	for (FCraftedSigil sigil : CreatedSigils)
	{
		if (sigil.SigilSettings.bGrabCounter && sigil.CraftedUnits > 0)
		{
			CurrentGrabCounter = sigil.SigilSettings;
			break;
		}
	}
}

TArray<FCraftableSigil> UComponent_Inventory::GetCraftableSigils()
{
	TArray<FCraftableSigil> CraftableSigils;

	for (FSigilIconSettings s : SigilsTableSettings)
	{
		int32 CraftableUnits = GetMaxCraftableUnit(s);
		if (CraftableUnits > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Craftable units greater than 0"));
			FCraftableSigil CraftableSigil;
			CraftableSigil.SigilIconSettings = s;
			CraftableSigil.CraftableUnits = CraftableUnits;
			CraftableSigils.Add(CraftableSigil);
		}
	}

	return CraftableSigils;
}

int32 UComponent_Inventory::GetMaxCraftableUnit(FSigilIconSettings TargetSigil)
{
	int32 ReqNum = TargetSigil.MaterialRequired;
	EMaterialType ReqMat = TargetSigil.MaterialType;

	int32 MatKey = static_cast<int32>(ReqMat);

	if (const int32* FoundValue = CraftingMaterials.Find(ReqMat))
	{
		return *FoundValue / ReqNum;
	}

	return 0;
}

void UComponent_Inventory::AddCraftingMaterial(EMaterialType NewMaterial, int32 Amount)
{
	int32 MatInt = static_cast<int32>(NewMaterial);
	if (int32* FoundValue = CraftingMaterials.Find(NewMaterial))
	{
		(*FoundValue) += Amount;
	}
	else
	{
		CraftingMaterials.Add(NewMaterial, 1);
	}

}

void UComponent_Inventory::AddKeyItem(FKeySettings NewKey)
{
	KeyItems.Add(NewKey);
}

ESigilIconState UComponent_Inventory::GetSigilState(ESigilAction SigilType)
{
	for (FCraftedSigil craftable : CreatedSigils)
	{
		if (craftable.SigilSettings.Effect == SigilType)
			return ESigilIconState::Available;
	}
	
	TArray<FCraftableSigil> CraftableSigils = GetCraftableSigils();

	for (FCraftableSigil craftable : CraftableSigils)
	{
		if (craftable.SigilIconSettings.SigilType == SigilType)
		{
			return ESigilIconState::Craftable;
		}
	}

	for (FSigilSettings poolElem : SigilPool)
	{
		if (poolElem.Effect == SigilType)
		{
			return ESigilIconState::NoMaterials;
		}
	}

	return ESigilIconState::Unknown;
}

FName UComponent_Inventory::GetSigilNameFromType(ESigilAction SigilType)
{
	for (FSigilSettings settings : SigilPool)
	{
		if (settings.Effect == SigilType)
			return settings.SigilName;
	}

	return FName();
}

bool UComponent_Inventory::GetIsEquipped(ESigilAction SigilType)
{
	if (SigilType == EquippedSigil.Effect )
		return true;

	return false;
}

ESigilAction UComponent_Inventory::GetEquippedSigilType()
{
	return EquippedSigil.Effect;
}

FName UComponent_Inventory::GetEquippedSigilName()
{
	return EquippedSigil.SigilName;
}

void UComponent_Inventory::TryCrafting(ESigilAction SigilType)
{
	EMaterialType ReqMat;
	int32 ReqUnit;
	
	for (FSigilIconSettings sigil : SigilsTableSettings)
	{
		if (sigil.SigilType == SigilType)
		{
			ReqMat = sigil.MaterialType;
			ReqUnit = sigil.MaterialRequired;

			if (HasRequiredMaterials(ReqMat, ReqUnit))
			{
				CraftSigil(SigilType, ReqMat, ReqUnit);
			}
		}
	}


}

bool UComponent_Inventory::HasRequiredMaterials(EMaterialType MaterialType, int32 UnitsNeeded)
{
	if (const int32* FoundValue = CraftingMaterials.Find(MaterialType))
	{
		return *FoundValue >= UnitsNeeded;
	}

	return false;
}

void UComponent_Inventory::CraftSigil(ESigilAction SigilType, EMaterialType MaterialType, int32 UnitsNeeded)
{
	CraftingMaterials[MaterialType] -= UnitsNeeded;
	
	int32 indexOfExistingSigil = GetAlreadyCreatedSigilIndex(SigilType);

	if (indexOfExistingSigil >= 0)
	{
		AddUnitToCreatedSigil(indexOfExistingSigil, SigilType, 1);
		return;
	}

	CreateNewSigil(SigilType, 1);

	//OnEquipSlotChange.Broadcast();
}

FSigilSettings UComponent_Inventory::GetSigilSettingsFromPool(ESigilAction SigilType)
{
	for (FSigilSettings settings : SigilPool)
	{
		if (settings.Effect == SigilType)
		{
			return settings;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Component_Inventory - GetSigilFromPool : attempt creating unknown sigil (not present in sigil pool)"));
	return FSigilSettings();
}


void UComponent_Inventory::EquipSigil(ESigilAction SigilType)
{
	EquippedSigil = GetSigilSettingsFromPool(SigilType);

	OnEquipSlotChange.Broadcast();
}

int32 UComponent_Inventory::GetAlreadyCreatedSigilIndex(ESigilAction SigilType)
{
	for (int32 Index = 0; Index < CreatedSigils.Num(); ++Index)
	{
		if (CreatedSigils[Index].SigilSettings.Effect == SigilType)
		{
			return Index;
		}
	}

	return -1;
}

void UComponent_Inventory::AddUnitToCreatedSigil(int32 CreatedSigilIndex, ESigilAction SigilType, int32 UnitsToAdd)
{
	CreatedSigils[CreatedSigilIndex].CraftedUnits += UnitsToAdd;
}

void UComponent_Inventory::CreateNewSigil(ESigilAction SigilType, int32 Units)
{
	FCraftedSigil NewSigil;
	NewSigil.SigilSettings = GetSigilSettingsFromPool(SigilType);
	NewSigil.CraftedUnits = Units;
	CreatedSigils.Add(NewSigil);
}

void UComponent_Inventory::ConsumeEquippedSigil()
{
	int32 EquippedSigilCreatedIndex = GetAlreadyCreatedSigilIndex(EquippedSigil.Effect);
	int32 Amount = 0;

	if (EquippedSigilCreatedIndex >= 0)
	{
		CreatedSigils[EquippedSigilCreatedIndex].CraftedUnits--;
		Amount = CreatedSigils[EquippedSigilCreatedIndex].CraftedUnits;
		OnEquipSlotChange.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("COMPONENT_INVENTORY - ConsumeEquippedSigil: EquippedSigil is not in CreatedSigil array, cannot consume"));
	}

	if (Amount <= 0)
	{
		CreatedSigils.RemoveAt(EquippedSigilCreatedIndex);

		if (CreatedSigils.Num() > 0)
		{
			EquipSigil(CreatedSigils[0].SigilSettings.Effect);
		}
		else
		{
			EmptyEquippedSigil();
			OnEquipSlotChange.Broadcast();
		}
	}

}

void UComponent_Inventory::EmptyEquippedSigil()
{
	EquippedSigil.SigilName = "None";
	EquippedSigil.Effect = ESigilAction::None;
	EquippedSigil.Montage = nullptr;
	EquippedSigil.bGrabCounter = false;
}

int32 UComponent_Inventory::GetAmountOfCreatedSigil(ESigilAction SigilType)
{
	for (FCraftedSigil created : CreatedSigils)
	{
		if (created.SigilSettings.Effect == SigilType)
		{
			return created.CraftedUnits;
		}
	}

	return 0;
}

void UComponent_Inventory::ChangeToNextSigil()
{
	if (CreatedSigils.Num() <= 0)
		return;

	int32 EquippedSigilIndex = GetAlreadyCreatedSigilIndex(EquippedSigil.Effect);

	int32 NextSigilIndex = EquippedSigilIndex++;

	if (NextSigilIndex > CreatedSigils.Num())
	{
		NextSigilIndex = 0;
	}

	FCraftedSigil NextSigil = CreatedSigils[NextSigilIndex];

	EquipSigil(NextSigil.SigilSettings.Effect);
}

void UComponent_Inventory::ConsumeGrabCounterSigil()
{
	int32 CounterSigilCreatedIndex = GetAlreadyCreatedSigilIndex(CurrentGrabCounter.Effect);
	int32 Amount = 0;

	if (CounterSigilCreatedIndex >= 0)
	{
		CreatedSigils[CounterSigilCreatedIndex].CraftedUnits--;
		Amount = CreatedSigils[CounterSigilCreatedIndex].CraftedUnits;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("COMPONENT_INVENTORY - ConsumeGrabCounterSigil: GrabCounter is not in CreatedSigil array, cannot consume"));
	}

	if (Amount <= 0)
	{
		CreatedSigils.RemoveAt(CounterSigilCreatedIndex);
		if (CurrentGrabCounter.Effect == EquippedSigil.Effect)
		{
			if (CreatedSigils.Num() > 0)
			{
				EquipSigil(CreatedSigils[0].SigilSettings.Effect);
			}
			else
			{
				EmptyEquippedSigil();
			}
		}

	}
	OnEquipSlotChange.Broadcast();
}



