// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_BaseCharacter.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_BaseCharacter : public UInterface
{
	GENERATED_BODY()
};

class ZERO_API IInterface_BaseCharacter
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	bool GetIsCrouching();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ForcedSetTransform(FTransform NewTransform);
};
