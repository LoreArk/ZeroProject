// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_Interactable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZERO_API IInterface_Interactable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void Interact(AActor* InteractingActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void EnterInteractionTargeting();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ExitInteractionTargeting();
};
