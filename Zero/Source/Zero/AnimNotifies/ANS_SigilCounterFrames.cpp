// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_SigilCounterFrames.h"
#include "Interface_Player.h"

void UANS_SigilCounterFrames::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp->GetOwner() == nullptr)
		return;

	if (bool Char = MeshComp->GetOwner()->Implements<UInterface_Player>())
	{
		IInterface_Player::Execute_SetGrabSigilEnabled(MeshComp->GetOwner(), true);
	}
}

void UANS_SigilCounterFrames::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp->GetOwner() == nullptr)
		return;

	if (bool Char = MeshComp->GetOwner()->Implements<UInterface_Player>())
	{
		IInterface_Player::Execute_SetGrabSigilEnabled(MeshComp->GetOwner(), false);
	}
}
