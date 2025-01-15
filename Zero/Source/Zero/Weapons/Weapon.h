// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component_ObjectPool.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Weapon.generated.h"

UCLASS()
class ZERO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UComponent_ObjectPool* ObjectPoolComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* FXTransformReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CarrySocketName;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TraceEndSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TraceStartSocketName;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FPTraceEndSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FPTraceStartSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* LeftHandTransform;

	UFUNCTION(BlueprintNativeEvent)
	void PlayFX();
	void PlayFX_Implementation();
};
