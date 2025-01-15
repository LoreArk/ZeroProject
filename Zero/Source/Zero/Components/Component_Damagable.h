// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageStructs.h"
#include "Component_Damagable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, DeadActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamage, AActor*, DamageDealer);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZERO_API UComponent_Damagable : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UComponent_Damagable();

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnDamage OnDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Mana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsInvincible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsIntangible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UnitFallDamageThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinFallDamageHeightThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinFallHeightForLanding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UAnimMontage*, FHitReactionSettings> HitReactions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* SoftLandMontage;

	UPROPERTY(BlueprintReadWrite)
	FDamageSettings LastDamageSettings;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool TakeDamage(FDamageSettings DamageSettings, AActor* DamageDealer);

	void FallDamage(float StartHeigth);

	void DamageResponse(EDamageResponse DamageResponse, AActor* DamageDealer);

};
