// Fill out your copyright notice in the Description page of Project Settings.


#include "DummyPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

void ADummyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ADummyPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ADummyPlayerCharacter::Look);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ADummyPlayerCharacter::Interact);

		EnhancedInputComponent->BindAction(CastDummyCharacter, ETriggerEvent::Completed, this, &ADummyPlayerCharacter::CastDummyActionCompleted);
	}
}

void ADummyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetInUse(false);
}

void ADummyPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	AliveTimerHandle.Invalidate();
}

void ADummyPlayerCharacter::SetEvasionEnabled(bool bEnabled, float TimeRemaining)
{
}

void ADummyPlayerCharacter::ToggleEvasionFlag_Implementation(bool bEnabled)
{
}

void ADummyPlayerCharacter::SetInUse_Implementation(bool InUse)
{
	bInUse = InUse;
	SetActorEnableCollision(bInUse);
	SetActorHiddenInGame(!bInUse);
	SetActorTickEnabled(bInUse);

	if (GetWorldTimerManager().IsTimerActive(AliveTimerHandle))
		GetWorldTimerManager().ClearTimer(AliveTimerHandle);

	AliveTimerHandle.Invalidate();

	if (bInUse)
	{
		GetWorldTimerManager().SetTimer(AliveTimerHandle, this, &ADummyPlayerCharacter::DisableActor, TimeToLive);
		/*GetWorld()->GetTimerManager().SetTimer(AliveTimerHandle, [this]()
			{
				ReturnToPool();
			}, TimeToLive, false);*/
	}
}

void ADummyPlayerCharacter::DisableActor()
{
	SetInUse(false);

	DamagableComponent->OnDeath.Broadcast(this);
	if (IsPlayerControlled())
	{
		ReturnToPlayer();
	}
}

void ADummyPlayerCharacter::EnableActor(FTransform SpawnTransform)
{
	if (bInUse)
		return;

	SetActorLocation(SpawnTransform.GetLocation());
	SetActorRotation(SpawnTransform.GetRotation());
	SetInUse(true);
}

void ADummyPlayerCharacter::CastDummyActionCompleted(const FInputActionInstance& Instance)
{
	UE_LOG(LogTemp, Warning, TEXT("DUMMY ACTION RETURN TO PLAYER"));

	GetCharacterMovement()->StopMovementImmediately();
	ReturnToPlayer();
}

void ADummyPlayerCharacter::ReturnToPlayer()
{
	if (APlayerCharacter* player = GetGameMode()->GetPlayerCharacter())
	{
		GetController()->Possess(player);
	}
}
