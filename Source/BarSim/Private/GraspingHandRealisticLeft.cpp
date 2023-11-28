// Fill out your copyright notice in the Description page of Project Settings.


#include "GraspingHandRealisticLeft.h"

#include "ActorInfoWidget.h"
#include "BarGameInstance.h"
#include "BarSpoon.h"
#include "BottleBase.h"
#include "Coaster.h"
#include "CupBase.h"
#include "DrinkCan.h"
#include "HalfSlicedLime.h"
#include "HalfSlicedLimeVat.h"
#include "HalfSlicedOrange.h"
#include "HalfSlicedOrangeVat.h"
#include "HuchuTong.h"
#include "IceCube.h"
#include "IceCubeVat.h"
#include "MartiniCup.h"
#include "MixingGlass.h"
#include "RockGlass.h"
#include "Shaker.h"
#include "ShakerLid.h"
#include "ShakerStrainer.h"
#include "SlicedLime.h"
#include "SlicedLimeVat.h"
#include "SlicedOrange.h"
#include "SlicedOrangeVat.h"
#include "Strainer.h"
#include "Tablet.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"

AGraspingHandRealisticLeft::AGraspingHandRealisticLeft(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	actorInfoWidgetComp=CreateDefaultSubobject<UWidgetComponent>(TEXT("actorInfoWidgetComp"));
	actorInfoWidgetComp->SetupAttachment(RootComponent);
}

void AGraspingHandRealisticLeft::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced Input 
	PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	
	actorInfoWidget=Cast<UActorInfoWidget>(actorInfoWidgetComp->GetWidget());
	
	skeletalHand=GetSkeletalMeshComponent();	
	skeletalHand->SetGenerateOverlapEvents(true);
	skeletalHand->OnComponentBeginOverlap.AddDynamic(this, &AGraspingHandRealisticLeft::OnOverlap);
	actorInfoWidgetComp->SetVisibility(false);
	BarGameInstance = Cast<UBarGameInstance>(GetWorld()->GetGameInstance());

	if(BarGameInstance)
	{
		BarGameInstance->resetHandDele.AddDynamic(this, &AGraspingHandRealisticLeft::RespawnGraspingHands);
	}
}

void AGraspingHandRealisticLeft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AGraspingHandRealisticLeft::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TArray<FHitResult> hits;
	if(SweepResult.GetNumOverlapHits(hits)>1)
	{
		FVector Center = skeletalHand->GetComponentLocation();
		int32 Closest = 0;
		for(int i=0; i<hits.Num(); ++i)
		{
			float ClosestDist = FVector::Dist(hits[Closest].GetActor()->GetActorLocation(), Center);
			float NextDist = FVector::Dist(hits[i].GetActor()->GetActorLocation(), Center);

			if (NextDist < ClosestDist)
			{
				Closest = i;
			}
		}

		OtherActor=hits[Closest].GetActor();
	}
	
	// Overlapped Actor Cast
	huchuTong=Cast<AHuchuTong>(OtherActor);
	bottle = Cast<ABottleBase>(OtherActor);
	tablet = Cast<ATablet>(OtherActor);
	coaster=Cast<ACoaster>(OtherActor);
	cup=Cast<ACupBase>(OtherActor);
	barSpoon=Cast<ABarSpoon>(OtherActor);
	strainer=Cast<AStrainer>(OtherActor);
	shakerStrainer=Cast<AShakerStrainer>(OtherActor);
	shakerLid=Cast<AShakerLid>(OtherActor);
	shaker=Cast<AShaker>(OtherActor);
	mixingGlass=Cast<AMixingGlass>(OtherActor);
	iceCube=Cast<AIceCube>(OtherActor);
	slicedLime=Cast<ASlicedLime>(OtherActor);
	halfSlicedLime=Cast<AHalfSlicedLime>(OtherActor);
	cup=Cast<ACupBase>(OtherActor);
	slicedLimeVat=Cast<ASlicedLimeVat>(OtherActor);
	halfSlicedLimeVat=Cast<AHalfSlicedLimeVat>(OtherActor);
	iceCubeVat=Cast<AIceCubeVat>(OtherActor);
	martiniCup=Cast<AMartiniCup>(OtherActor);
	rockGlass=Cast<ARockGlass>(OtherActor);
	SlicedOrange=Cast<ASlicedOrange>(OtherActor);
	halfSlicedOrange=Cast<AHalfSlicedOrange>(OtherActor);
	SlicedOrangeVat=Cast<ASlicedOrangeVat>(OtherActor);
	halfSlicedOrangeVat=Cast<AHalfSlicedOrangeVat>(OtherActor);
	DrinkCan=Cast<ADrinkCan>(OtherActor);
	
		if(huchuTong)
		{
			if(PC)
			{
				PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
			}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT(" Tongs"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("집게"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false);
	}
	else if(barSpoon)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Bar Spoon"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("바 스푼"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(tablet)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Tablet"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("태블릿"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(coaster)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Coaster"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("코스터"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(strainer)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Strainer"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("스트레이너"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(shakerStrainer)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Shaker Strainer"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("쉐이커 스트레이너"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(shakerLid)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Shaker Lid"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("쉐이커 뚜껑"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(shaker)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Shaker"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("쉐이커"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(mixingGlass)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Mixing Glass"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("믹싱 글래스"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(iceCube)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Ice Cube"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("얼음"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(halfSlicedLime)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Lime"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("라임"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(slicedLime)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Lime"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("라임"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(slicedLimeVat)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Lime Container"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("라임 컨테이너"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(halfSlicedLimeVat)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Lime Container"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("라임 컨테이너"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(halfSlicedOrangeVat)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Orange Container"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("오렌지 컨테이너"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(SlicedOrangeVat)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Orange Container"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("오렌지 컨테이너"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(SlicedOrange)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Orange"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("오렌지"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(halfSlicedOrange)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Orange"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("오렌지"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(iceCubeVat)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Ice Container"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("아이스 컨테이너"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(martiniCup)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Martini Glass"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("마티니 글래스"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(rockGlass)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Rock Glass"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("온더록 글래스"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(cup)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Highball Glass"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("하이볼 글래스"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }
	else if(DrinkCan)
	{
		if(PC)
		{
			PC->PlayHapticEffect(HF_ActorOverlap, EControllerHand::Left);			
		}
		GetWorldTimerManager().ClearTimer(widgetDestroyHandle);
		if(actorInfoWidget->IsAnimationPlaying(actorInfoWidget->Disappearing))
		{
			actorInfoWidget->StopAnimation(actorInfoWidget->Disappearing);
		}
		if(BarGameInstance&&BarGameInstance->bEnglish==true)
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("Drink Can"))));
		}
		else
		{
			actorInfoWidget->ActorInfo->SetText(FText::FromString(FString::Printf(TEXT("캔 음료"))));
		}
		actorInfoWidgetComp->SetVisibility(true);
		actorInfoWidget->PlayAnimation(actorInfoWidget->Appearing);
		GetWorldTimerManager().SetTimer(widgetDestroyHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			actorInfoWidget->PlayAnimation(actorInfoWidget->Disappearing);
		}), 1, false); }

	
}


