// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryStructs.h"
#include "CraftingMaterial.h"
#include "KeyItem.h"
#include "Components/ActorComponent.h"
#include "Component_Inventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquippedSlotChange);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZERO_API UComponent_Inventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UComponent_Inventory();

protected:
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FKeySettings> KeyItems;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnEquippedSlotChange OnEquipSlotChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSigilSettings EquippedSigil;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSigilSettings> SigilPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCraftedSigil> CreatedSigils;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSigilIconSettings> SigilsTableSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EMaterialType, int32> CraftingMaterials; //Key is EMaterialType, Value is the amount 

	UFUNCTION()
	void SetGrabCounter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSigilSettings CurrentGrabCounter;

	UFUNCTION(BlueprintCallable)
	TArray<FCraftableSigil> GetCraftableSigils();

	UFUNCTION(BlueprintCallable)
	int32 GetMaxCraftableUnit(FSigilIconSettings TargetSigil);

	UFUNCTION(BlueprintCallable)
	void AddCraftingMaterial(EMaterialType NewMaterial, int32 Amount);

	UFUNCTION(BlueprintCallable)
	void AddKeyItem(FKeySettings NewKey);

	UFUNCTION(BlueprintCallable)
	ESigilIconState GetSigilState(ESigilAction SigilType);

	UFUNCTION(BlueprintCallable)
	FName GetSigilNameFromType(ESigilAction SigilType);

	UFUNCTION(BlueprintCallable)
	bool GetIsEquipped(ESigilAction SigilType);

	UFUNCTION(BlueprintCallable)
	ESigilAction GetEquippedSigilType();

	UFUNCTION(BlueprintCallable)
	FName GetEquippedSigilName();
	
	UFUNCTION(BlueprintCallable)
	void TryCrafting(ESigilAction SigilType);

	UFUNCTION(BlueprintCallable)
	bool HasRequiredMaterials(EMaterialType MaterialType, int32 UnitsNeeded);

	UFUNCTION()
	void CraftSigil(ESigilAction SigilType, EMaterialType MaterialType, int32 UnitsNeeded);
	
	UFUNCTION()
	FSigilSettings GetSigilSettingsFromPool(ESigilAction SigilType);

	UFUNCTION(BlueprintCallable)
	void EquipSigil(ESigilAction SigilType);

	UFUNCTION()
	int32 GetAlreadyCreatedSigilIndex(ESigilAction SigilType);

	UFUNCTION()
	void AddUnitToCreatedSigil(int32 CreatedSigilIndex, ESigilAction SigilType, int32 UnitsToAdd);

	UFUNCTION()
	void CreateNewSigil(ESigilAction SigilType, int32 Units);

	UFUNCTION()
	void ConsumeEquippedSigil();

	UFUNCTION()
	void EmptyEquippedSigil();

	UFUNCTION(BlueprintCallable)
	int32 GetAmountOfCreatedSigil(ESigilAction SigilType);

	UFUNCTION()
	void ChangeToNextSigil();

	UFUNCTION()
	void ConsumeGrabCounterSigil();
};
