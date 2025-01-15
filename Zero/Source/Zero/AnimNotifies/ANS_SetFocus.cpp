// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_SetFocus.h"
#include "EnemyCharacter.h"
#include "Interface_AIAgent.h"

void UANS_SetFocus::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOwner() == nullptr)
		return;


	if (bool Char = MeshComp->GetOwner()->Implements<UInterface_AIAgent>())
	{
		// Call your function to start the action
		IInterface_AIAgent::Execute_SetAttackFocus(MeshComp->GetOwner());
		//AEnemyCharacter* enemy = IInterface_Enemy::Execute_GetOwnerCharacter(MeshComp->GetOwner());
		//enemy->GetCharacterMovement()->bAllow
	}
}

void UANS_SetFocus::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp->GetOwner() == nullptr)
		return;

	if (bool Char = MeshComp->GetOwner()->Implements<UInterface_AIAgent>())
	{
		// Call your function to start the action
		IInterface_AIAgent::Execute_ClearAttackFocus(MeshComp->GetOwner());
	}
}
