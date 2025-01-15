// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "InventoryStructs.h"
#include "CraftingMaterial.generated.h"

/**
 * 
 */
UCLASS()
class ZERO_API ACraftingMaterial : public AItem
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMaterialType MaterialType;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount;


	virtual void Interact_Implementation(AActor* InteractingActor) override;

};
