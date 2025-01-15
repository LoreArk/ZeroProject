// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageStructs.h"
#include "Interface_Damagable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_Damagable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZERO_API IInterface_Damagable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	bool TakeDamage(FDamageSettings DamageSettings, FHitResult HitResult, AActor* DamageDealer);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	void PlayHitReaction(UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	void PlayFallHitReaction(UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	UComponent_Damagable* GetDamagableComponent();


};
