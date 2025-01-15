// Fill out your copyright notice in the Description page of Project Settings.


#include "PA_Sigil.h"

void APA_Sigil::SetDamageSettings_Implementation(FDamageSettings Settings, AActor* TargetActor)
{
	DamageSettings = Settings;
	DamageTarget = TargetActor;

}

APA_Sigil* APA_Sigil::GetSigilPooledActor_Implementation()
{
	return this;
}
