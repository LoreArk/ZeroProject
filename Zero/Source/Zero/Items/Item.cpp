// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "ZeroGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "InteractableStateActor.h"
#include "Interface_Player.h"

// Sets default values
AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;

    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    RootComponent = SceneComponent;
    StaticMesh->SetupAttachment(SceneComponent);

    StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
    SphereCollider->SetupAttachment(SceneComponent);

    SphereCollider->SetSphereRadius(100.0f);

    SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    SphereCollider->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

    SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnBeginOverlap);
    SphereCollider->OnComponentEndOverlap.AddDynamic(this, &AItem::OnEndOverlap);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	

}

void AItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->Implements<UInterface_Player>())
    {
        UE_LOG(LogTemp, Warning, TEXT("ITEM OVERLAPPED PLAYER"));
        IInterface_Player::Execute_SetInteractionTarget(OtherActor, this);
        OnPlayerOverlap(OtherActor);
    }
}

void AItem::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor->Implements<UInterface_Player>())
    {
        UE_LOG(LogTemp, Warning, TEXT("ITEM END OVERLAPPING PLAYER"));
        IInterface_Player::Execute_SetInteractionTarget(OtherActor, nullptr);
    }
}

void AItem::OnPlayerOverlap_Implementation(AActor* Actor)
{

}

void AItem::SetState_Implementation(bool NewState)
{
    Super::SetState_Implementation(NewState);

    if (bEnabled)
    {
        SetActorHiddenInGame(false);
        SphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    else
    {
        SetActorHiddenInGame(true);
        SphereCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void AItem::Interact_Implementation(AActor* InteractingActor)
{
    SetState(false);
    IInterface_Player::Execute_SetInteractionTarget(InteractingActor, nullptr);
}

