// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BarGameInstance.generated.h"

DECLARE_DELEGATE(FGoMainDele);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResetHandDele);

/**
 * 
 */
UCLASS()
class BARSIM_API UBarGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	
	FGoMainDele goToMainMapDele;
	FResetHandDele resetHandDele;

	UPROPERTY()
	class AGameStartManager* startManager;

	UFUNCTION()
	void CheckDay();

	UFUNCTION()
	void BindFunction();

	UFUNCTION()
	void ResetHandBroadcast();

	// 날짜
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 checkDayCount; // 0

	// 아케이드 스토리 모드 체크
	UPROPERTY()
	bool bCheckArcadeMode; // false
	
	// 튜토리얼 체크 불
	UPROPERTY()
	bool bCheckTutorialMode; // false

	UPROPERTY()
	bool bCheckMenu;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnglish; //false

	// 첫째날 / 둘째날 / 셋째날 / 모든날
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<float> TotalMoney = {0, 0, 0, 0};

	UFUNCTION()
	void AddMoney(float money);	
};
