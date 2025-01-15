// Fill out your copyright notice in the Description page of Project Settings.


#include "GunnerHUD.h"
#include "EnemyGunner.h"

void UGunnerHUD::SetDamagableComponent(UComponent_Damagable* DamagableComp)
{
	DamagableComponent = DamagableComp;

	if (DamagableComponent.IsValid())
	{
		DamagableComponent->OnDamage.AddDynamic(this, &UCharacterHUD::PlayDamageEffect);

		if (auto* character = Cast<AEnemyGunner>(DamagableComponent->GetOwner()))
		{
			MaxShots = character->Shots;
			UpdateAmmo(MaxShots);
			character->OnFPShoot.AddDynamic(this, &UGunnerHUD::UpdateAmmo);
		}
	}

}

void UGunnerHUD::UpdateAmmo_Implementation(int32 Shots)
{

}
