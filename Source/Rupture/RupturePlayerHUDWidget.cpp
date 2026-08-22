// Fill out your copyright notice in the Description page of Project Settings.


#include "RupturePlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void URupturePlayerHUDWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	// Cláusula de guarda para evitar divisão por zero e checar se a barra existe
	if (pb_Health && MaxHealth > 0.f)
	{
		pb_Health->SetPercent(CurrentHealth / MaxHealth);
	}
}

void URupturePlayerHUDWidget::UpdateAmmo(int32 CurrentAmmo, int32 ReserveAmmo)
{
	// Verifica se os textos não são nulos antes de alterar
	if (txt_CurrentAmmo && txt_ReserveAmmo)
	{
		txt_CurrentAmmo->SetText(FText::AsNumber(CurrentAmmo));
		txt_ReserveAmmo->SetText(FText::AsNumber(ReserveAmmo));
	}
}