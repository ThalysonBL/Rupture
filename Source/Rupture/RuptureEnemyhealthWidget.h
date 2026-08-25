#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RuptureEnemyhealthWidget.generated.h"

class UProgressBar;

UCLASS()
class RUPTURE_API URuptureEnemyhealthWidget: public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* pb_Health;
	void UpdateHealth(const float CurrentHealth, const float MaxHealth);
	void ShowBar();
	void HideBar();	
};
