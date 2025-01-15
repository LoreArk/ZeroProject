// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_EnableEvadeFlag.h"
#include "Interface_Enemy.h"

void UANS_EnableEvadeFlag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);



	if (MeshComp->GetOwner())
	{
		if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
		{
			IInterface_Enemy::Execute_ToggleEvasionFlag(MeshComp->GetOwner(), true);
		}
	}

}

void UANS_EnableEvadeFlag::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp->GetOwner())
	{
		if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
		{
			IInterface_Enemy::Execute_ToggleEvasionFlag(MeshComp->GetOwner(), true);
		}
	}
}

void UANS_EnableEvadeFlag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp->GetOwner())
	{
		if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UInterface_Enemy::StaticClass()))
		{
			IInterface_Enemy::Execute_ToggleEvasionFlag(MeshComp->GetOwner(), false);
		}
	}
}
