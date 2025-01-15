// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageStructs.h"
#include "Interface_AIAgent.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInterface_AIAgent : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZERO_API IInterface_AIAgent
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	float SetMovementSpeed(EGait Speed);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	AActor* GetAttackTarget();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void Attack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void AttackTraceStart(FDamageSettings DamageSettings);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void AttackTraceTick();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void AttackTraceEnd();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SetAttackFocus();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ClearAttackFocus();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SetStateAsPassive();
};
