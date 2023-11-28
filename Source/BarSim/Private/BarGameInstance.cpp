// Fill out your copyright notice in the Description page of Project Settings.


#include "BarGameInstance.h"
#include "GameStartManager.h"
#include "Tablet.h"
#include "TabletWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UBarGameInstance::Init()
{
	Super::Init();

	// 스타트 매니저 캐스트
	startManager = Cast<AGameStartManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameStartManager::StaticClass()));
	
	//델리게이트 연결 함수 실행
	BindFunction();

	resetHandDele.AddDynamic(this, &UBarGameInstance::ResetHandBroadcast);

	
}

void UBarGameInstance::CheckDay()
{
	// 메인맵으로 넘어 갈때마다 1씩 올라가게 한다
	if(bCheckTutorialMode != false && bCheckArcadeMode != true)
	{
		checkDayCount++;
	}
	else if(bCheckTutorialMode != false && bCheckArcadeMode != false)
	{
		checkDayCount = checkDayCount + 3;
	}
	
}

void UBarGameInstance::BindFunction()
{
	// 체크데이 함수 델리케이트에 연결
	goToMainMapDele.BindUFunction(this, FName("CheckDay"));
}

void UBarGameInstance::ResetHandBroadcast()
{
	
}

void UBarGameInstance::AddMoney(float money)
{
	auto tablet = Cast<ATablet>(UGameplayStatics::GetActorOfClass(GetWorld(), ATablet::StaticClass()));
	
	if(checkDayCount == 1)
	{
		TotalMoney[0] += money;

		TotalMoney[3] += money;

		tablet->tablet_UI->todayMoney->SetText(FText::AsNumber(TotalMoney[0]));

		tablet->tablet_UI->totalMoney->SetText(FText::AsNumber(TotalMoney[3]));
	}
	else if(checkDayCount == 2)
	{
		TotalMoney[1] += money;

		TotalMoney[3] += money;

		tablet->tablet_UI->todayMoney->SetText(FText::AsNumber(TotalMoney[1]));

		tablet->tablet_UI->totalMoney->SetText(FText::AsNumber(TotalMoney[3]));
	}
	else if(checkDayCount == 3)
	{
		TotalMoney[2] += money;

		TotalMoney[3] += money;

		tablet->tablet_UI->todayMoney->SetText(FText::AsNumber(TotalMoney[2]));

		tablet->tablet_UI->totalMoney->SetText(FText::AsNumber(TotalMoney[3]));
	}
}
