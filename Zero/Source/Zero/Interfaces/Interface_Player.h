// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interface_Player.generated.h"

UINTERFACE(MinimalAPI)
class UInterface_Player : public UInterface
{
	GENERATED_BODY()
};

class ZERO_API IInterface_Player
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void CastSigilOfType();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SetInteractionTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void StartCatStepsEffect(float Time);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ToggleEvasionFlag(bool bEnabled);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	bool IsEvasionEnabled();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void AddMana(int32 ManaAmount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ConsumeMana(int32 Amount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void StabAction(AActor* Victim);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ReceiveGrabAttack(AActor* EnemyActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void GrabAttackInterrupted();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void SetGrabSigilEnabled(bool bEnable);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces")
	void ShrineInteraction(int32 ManaToAdd);


};
