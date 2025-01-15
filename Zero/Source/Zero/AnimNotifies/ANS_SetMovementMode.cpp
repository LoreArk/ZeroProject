// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_SetMovementMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

void UANS_SetMovementMode::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();

		if (ACharacter* Char = Cast<ACharacter>(OwnerActor))
		{
			Char->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		}
	}

}

void UANS_SetMovementMode::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();

		if (ACharacter* Char = Cast<ACharacter>(OwnerActor))
		{
			Char->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		}
	}
}
