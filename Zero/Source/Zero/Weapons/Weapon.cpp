// Fill out your copyright notice in the Description page of Project Settings.



#include "Weapon.h"
#include "Component_ObjectPool.h"
#include "Components/ArrowComponent.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	RootComponent = SceneRoot;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Static Mesh"));
	WeaponMesh->SetupAttachment(SceneRoot);

	ObjectPoolComponent = CreateDefaultSubobject<UComponent_ObjectPool>(TEXT("Object Pool Component"));

	FXTransformReference = CreateDefaultSubobject<UArrowComponent>(TEXT("FX Transform Reference"));
	FXTransformReference->SetupAttachment(WeaponMesh);

	LeftHandTransform = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftHandIKTransform"));
	LeftHandTransform->SetupAttachment(WeaponMesh);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}


void AWeapon::PlayFX_Implementation()
{
}

