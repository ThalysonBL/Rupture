// Fill out your copyright notice in the Description page of Project Settings.


#include "RupturePlayerHUDWidget.h"
#include "Components/ProgressBar.h"

void URupturePlayerHUDWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	// Cláusula de guarda para evitar divisão por zero e checar se a barra existe
	if (pb_Health && MaxHealth > 0.f)
	{
		pb_Health->SetPercent(CurrentHealth / MaxHealth);
	}
}