// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_Ally.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_Ally : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZERO_API IInterface_Ally
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SetMoveToTaskLocation(FVector Location);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SetAttackTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void StopMoveToTask();


};
