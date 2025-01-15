// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Component_Perceivable.h"
#include "Interface_Perception.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_Perception : public UInterface
{
	GENERATED_BODY()
};

class ZERO_API IInterface_Perception
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	UComponent_Perception* GetPerceptionComponent();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	UComponent_Perceivable* GetPerceivableComponent();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	FVector GetSightTarget();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	FTransform GetSightOriginTransform();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SeenNewActor(AActor* SeenActor);

};
