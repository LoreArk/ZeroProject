// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyCharacter.h"
#include "Interface_Interactable.h"
#include "Shrine.generated.h"

UCLASS()
class ZERO_API AShrine : public AActor, public IInterface_Interactable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShrine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mana Settings")
	int32 SoulsCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mana Settings")
	int32 SoulsForManaUnit;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mana Settings")
	int32 AvailableMana;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mana Settings")
	int32 AvailableManaCap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mana Settings")
	TArray<AActor*> Enemies;

	UPROPERTY()
	int32 MaxManaRequested;

	UFUNCTION()
	void AddSouls(AActor* DeadActor);

	UFUNCTION()
	void SetAvailableMana();

	UFUNCTION(BlueprintNativeEvent)
	void UpdateManaVFX();
	void UpdateManaVFX_Implementation();

	UFUNCTION()
	void DisplayHUD();

	UFUNCTION(BlueprintNativeEvent)
	void InitManaVFX();
	void InitManaVFX_Implementation();

	UFUNCTION()
	void UpdateSoulsCount(int32 ManaToSubtract);

	UFUNCTION(BlueprintNativeEvent)
	void EnemyDeath(AActor* EnemyActor);
	void EnemyDeath_Implementation(AActor* EnemyActor);

	//Interactable interface

	virtual void Interact_Implementation(AActor* InteractingActor) override;
	virtual void EnterInteractionTargeting_Implementation() override;
	virtual void ExitInteractionTargeting_Implementation() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveSystem")
	FString UniqueID;
};
