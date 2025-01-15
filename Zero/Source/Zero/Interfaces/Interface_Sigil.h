// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageStructs.h"
#include "UObject/Interface.h"
#include "Interface_Sigil.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_Sigil : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZERO_API IInterface_Sigil
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SetDamageSettings(FDamageSettings Settings, AActor* TargetActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	APA_Sigil* GetSigilPooledActor();
};
