// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoundTypes.generated.h"

USTRUCT(BlueprintType)
struct  FRoundDefinition
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Round", meta = (ClampMin = "0"))
	int32 RiflemanCount = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Round", meta = (ClampMin = "0"))
	int32 BrawlerCount = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Round", meta = (ClampMin = "0.0"))
	float DelayAfterClear = 3.0f;
};
