// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

#include "BarGameInstance.h"
#include "BarGameMode.h"
#include "BarSpoon.h"
#include "BottleBase.h"
#include "Coaster.h"
#include "CupBase.h"
#include "DrinkCan.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GripMotionControllerComponent.h"
#include "GuideWidgetActor.h"
#include "HalfSlicedLime.h"
#include "HalfSlicedLimeVat.h"
#include "HalfSlicedOrange.h"
#include "HalfSlicedOrangeVat.h"
#include "HuchuTong.h"
#include "IceCube.h"
#include "IceCubeVat.h"
#include "MixingGlass.h"
#include "NiagaraComponent.h"
#include "OlivePick.h"
#include "OliveVat.h"
#include "PlayerDialogWidget.h"
#include "Shaker.h"
#include "ShakerLid.h"
#include "ShakerStrainer.h"
#include "SlicedLime.h"
#include "SlicedLimeVat.h"
#include "SlicedOrange.h"
#include "SlicedOrangeVat.h"
#include "Strainer.h"
#include "Tablet.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


APlayerCharacter::APlayerCharacter()
{

	widgetInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("widgetInteractionComp"));
	widgetInteractionComp->SetupAttachment(RightMotionController);
	widgetInteractionComp->SetRelativeLocation(FVector(9.1321, -2.7411, -8.8955));
	widgetInteractionComp->SetRelativeRotation(FRotator(-59.678, -36.935, 52.896));

	widgetInteractionCompLeft = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("widgetInteractionCompLeft"));
	widgetInteractionCompLeft->SetupAttachment(LeftMotionController);
	widgetInteractionCompLeft->SetRelativeLocation(FVector(9.0641, 5.1962, -10.4361));
	widgetInteractionCompLeft->SetRelativeRotation(FRotator(-52.5329, 21.7898, -2.4338));

	WidgetTraceRight=CreateDefaultSubobject<UNiagaraComponent>(TEXT("WidgetTraceRight"));
	WidgetTraceRight->SetupAttachment(widgetInteractionComp);

	WidgetTraceLeft=CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraWidgetTraceLeft"));
	WidgetTraceLeft->SetupAttachment(widgetInteractionCompLeft);

	playerTextWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("playerTextWidget"));
	playerTextWidget->SetupAttachment(VRReplicatedCamera);
}


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced Input 
	PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		// LocalPlayer
		auto localPlayer = PC->GetLocalPlayer();
		auto subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(localPlayer);
		if (subSystem)
		{
			subSystem->AddMappingContext(VREInputMappingContext, 0);
			subSystem->AddMappingContext(IMC_Hand, 0);
		}
	}

	//GetCharacterMovement()->bRequestedMoveUseAcceleration=false;
	//GetCharacterMovement()->bNetworkSkipProxyPredictionOnNetUpdate=true;
	
	widgetInteractionComp->bEnableHitTesting=true;
	widgetInteractionComp->bShowDebug=false;	
	widgetInteractionComp->InteractionDistance=300.0f;

	widgetInteractionCompLeft->bEnableHitTesting=true;
	widgetInteractionCompLeft->bShowDebug=false;	
	widgetInteractionCompLeft->InteractionDistance=300.0f;

	WidgetTraceLeft->SetVisibility(false);
	WidgetTraceRight->SetVisibility(false);

	gameMode = Cast<ABarGameMode>(GetWorld()->GetAuthGameMode());

	playerText_UI = Cast<UPlayerDialogWidget>(playerTextWidget->GetUserWidgetObject());

	BarGameInstance = Cast<UBarGameInstance>(GetWorld()->GetGameInstance());

	if(BarGameInstance)
	{
		BarGameInstance->resetHandDele.AddDynamic(this, &APlayerCharacter::ResetGraspingHands);
	}

	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &APlayerCharacter::PlayerStartText, 3.0f, false);
}



void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Tongs가 nullptr이 아니면서
	if(huchuTong!=nullptr)
	{
		// 오른손에 Tongs를 쥐고 있다면
		if(isGrabbingTongsRight)
		{
			// Oculus Trigger Input Value에 따른 Tongs Rotation 제어 Tick 활성화가 되어있다면
			if(isTongsTickEnabled == true)
			{
				huchuTong->tongRight->SetRelativeRotation(FRotator(fingerPressedActionValue*15, 0, 0));
			}
			
		}
	}
	// Tongs가 nullptr이 아니면서
	if(huchuTongL!=nullptr)
	{
		// 왼손에 Tongs를 쥐고 있다면
		if(isGrabbingTongsLeft)
		{
			// Oculus Trigger Input Value에 따른 Tongs Rotation 제어 Tick 왼쪽 활성화가 되어있다면
			if(isTongsTickEnabledL == true)
			{
				huchuTongL->tongRight->SetRelativeRotation(FRotator(fingerPressedActionValueLeft*15, 0, 0));
			}
			
		}
	}

	// Tablet Widget Line Trace
	bool bOverWidget = widgetInteractionComp->IsOverHitTestVisibleWidget();
	if(bOverWidget)
	{
		auto hitDist = widgetInteractionComp->GetLastHitResult().Distance;
		WidgetTraceRight->SetVectorParameter(FName("LaserEnd"), FVector(hitDist-3, 0, 0));
		WidgetTraceRight->SetVisibility(true);
	}
	else
	{
		WidgetTraceRight->SetVisibility(false);
	}
	// Tablet Widget Line Trace Left
	bool bOverWidgetLeft = widgetInteractionCompLeft->IsOverHitTestVisibleWidget();
	if(bOverWidgetLeft)
	{
		auto hitDist = widgetInteractionCompLeft->GetLastHitResult().Distance;
		WidgetTraceLeft->SetVectorParameter(FName("LaserEnd"), FVector(hitDist-3, 0, 0));
		WidgetTraceLeft->SetVisibility(true);
	}
	else
	{
		WidgetTraceLeft->SetVisibility(false);	
	}

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	auto InputSystem = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (InputSystem)
	{
		//Binding
		InputSystem->BindAction(UseHeldObjectLeft, ETriggerEvent::Started, this, &APlayerCharacter::FireLeft);
		InputSystem->BindAction(UseHeldObjectRight, ETriggerEvent::Started, this, &APlayerCharacter::FireRight);
		InputSystem->BindAction(UseHeldObjectLeft, ETriggerEvent::Completed, this, &APlayerCharacter::FireReleasedLeft);
		InputSystem->BindAction(UseHeldObjectRight, ETriggerEvent::Completed, this, &APlayerCharacter::FireReleasedRight);
		InputSystem->BindAction(ShowMenu, ETriggerEvent::Started, this, &APlayerCharacter::ShowMenuWidget);
		InputSystem->BindAction(ShowGuide, ETriggerEvent::Started, this, &APlayerCharacter::ShowGuideWidget);
		InputSystem->BindAction(ShowGuide, ETriggerEvent::Completed, this, &APlayerCharacter::HideGuideWidget);
		InputSystem->BindAction(TestInput, ETriggerEvent::Completed, this, &APlayerCharacter::TestInputKey);

	}
}

void APlayerCharacter::CheckGrabbedObjectRight()
{
	// Grabbing Sound
	UGameplayStatics::PlaySound2D(GetWorld(), grabSound, 1.2, 1, 0);
	
	// Haptic Feedback
	if(PC)
	{
		PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Right);			
	}
	
	TArray<AActor*> RightGrippedActors;
	RightMotionController->GetGrippedActors(RightGrippedActors);
	for(int i=0; i<RightGrippedActors.Num(); ++i)
	{
		GrabbedActorRight = RightGrippedActors[i];
	}

	// Right Grabbed Actor Casting
	huchuTong=Cast<AHuchuTong>(GrabbedActorRight);
	bottle = Cast<ABottleBase>(GrabbedActorRight);
	tablet = Cast<ATablet>(GrabbedActorRight);
	coaster=Cast<ACoaster>(GrabbedActorRight);
	cup=Cast<ACupBase>(GrabbedActorRight);
	barSpoon=Cast<ABarSpoon>(GrabbedActorRight);
	strainer=Cast<AStrainer>(GrabbedActorRight);
	shakerStrainer=Cast<AShakerStrainer>(GrabbedActorRight);
	shakerLid=Cast<AShakerLid>(GrabbedActorRight);
	shaker=Cast<AShaker>(GrabbedActorRight);
	mixingGlass=Cast<AMixingGlass>(GrabbedActorRight);
	drinkCan = Cast<ADrinkCan>(GrabbedActorRight);
	slicedLimeVat = Cast<ASlicedLimeVat>(GrabbedActorRight);
	halfSlicedLimeVat = Cast<AHalfSlicedLimeVat>(GrabbedActorRight);
	slicedOrangeVat = Cast<ASlicedOrangeVat>(GrabbedActorRight);
	halfSlicedOrangeVat = Cast<AHalfSlicedOrangeVat>(GrabbedActorRight);
	oliveVat = Cast<AOliveVat>(GrabbedActorRight);
	iceCubeVat = Cast<AIceCubeVat>(GrabbedActorRight);
	
		// 잡은 대상이 Tongs라면
		if(GrabbedActorRight==huchuTong&&huchuTong!=nullptr)
		{
			isGrabbingTongsRight=true;
			GetWorld()->GetTimerManager().ClearTimer(huchuTong->simHandle);
		}
		// 잡은 대상이 Bottle 이라면
		else if(GrabbedActorRight == bottle&&bottle!=nullptr)
		{
			isGrabbingBottleRight = true;
			GetWorld()->GetTimerManager().ClearTimer(bottle->simHandle);
			bottle->SetActorTickEnabled(true);
			bottle->isGrabbingBottle=true;
			bottle->isGBR=true;
		}
		// 잡은 대상이 Tablet 이라면
		else if(GrabbedActorRight==tablet&&tablet!=nullptr)
		{
			isGrabbingTabletRight=true;
			GetWorld()->GetTimerManager().ClearTimer(tablet->simHandle);
			tablet->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			tablet->VRGripInterfaceSettings.bSimulateOnDrop=true;
			//widgetInteractionComp->bShowDebug=false;
			//widgetInteractionCompLeft->bShowDebug=true;
		}		
		// 잡은 대상이 Coaster이라면
		else if(GrabbedActorRight==coaster&&coaster!=nullptr)
		{
			isGrabbingCoasterRight=true;
			GetWorld()->GetTimerManager().ClearTimer(coaster->simHandle);
			coaster->boxComp->SetCollisionProfileName(FName("CoasterAfterGrab"));
		}
	// 잡은 대상이 Shaker이라면
	else if(GrabbedActorRight==shaker&&shaker!=nullptr)
	{
		isGrabbingShakerRight=true;
		shaker->isGSR=true;
		shaker->SetActorTickEnabled(true);			
		shaker->isGrabbingShaker=true;
		shaker->VRGripInterfaceSettings.bDenyGripping=true;			
	}
		// 잡은 대상이 Cup이라면
		else if(GrabbedActorRight==cup&&cup!=nullptr)
		{
			isGrabbingCupRight=true;
			GetWorld()->GetTimerManager().ClearTimer(cup->simHandle);
			cup->SetActorTickEnabled(true);
			cup->isCupTickActivated=true;
		}
		// 잡은 대상이 BarSpoon이라면
		else if(GrabbedActorRight==barSpoon&&barSpoon!=nullptr)
		{
			isGrabbingBarSpoonRight=true;
			barSpoon->meshComp->SetCollisionProfileName(FName("SpoonGrabbed"));
		}

		// 잡은 대상이 ShakerLid라면
		else if(GrabbedActorRight==shakerLid&&shakerLid!=nullptr)
		{
			isGrabbingShakerLidRight=true;
			GetWorld()->GetTimerManager().ClearTimer(shakerLid->simHandle);
			shakerLid->SetLidOff();
			shakerLid->VRGripInterfaceSettings.bSimulateOnDrop=true;
			shakerLid->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
		// 잡은 대상이 ShakerStrainer 이라면
		else if(GrabbedActorRight==shakerStrainer&&shakerStrainer!=nullptr)
		{
			isGrabbingShakerStrainerRight=true;
			GetWorld()->GetTimerManager().ClearTimer(shakerStrainer->simHandle);
			shakerStrainer->VRGripInterfaceSettings.bSimulateOnDrop=true;
			shakerStrainer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
		// 잡은 대상이 MixingGlass 라면
		else if(GrabbedActorRight==mixingGlass&&mixingGlass!=nullptr)
		{
			isGrabbingMixingGlassRight=true;
			mixingGlass->SetActorTickEnabled(true);
			mixingGlass->isGrabbingMixingGlass=true;
			mixingGlass->VRGripInterfaceSettings.bDenyGripping=true;
			mixingGlass->VRGripInterfaceSettings.bSimulateOnDrop=true;
		}
		// 잡은 대상이 Glass Strainer 이라면
		else if(GrabbedActorRight==strainer&&strainer!=nullptr)
		{
			isGrabbingStrainerRight=true;
			GetWorld()->GetTimerManager().ClearTimer(strainer->simHandle);
			strainer->VRGripInterfaceSettings.bSimulateOnDrop=true;
			strainer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
		// 잡은 대상이 DrinkCan 이라면
		else if(GrabbedActorRight==drinkCan&&drinkCan!=nullptr)
		{
			isGrabbingDrinkCanRight = true;
		}
		// 잡은 대상이 halfSlicedLimeVat 이라면
		else if(GrabbedActorRight==halfSlicedLimeVat&&halfSlicedLimeVat!=nullptr)
		{
			isGrabbingHalfSlicedLimeVatRight = true;
			GetWorld()->GetTimerManager().ClearTimer(halfSlicedLimeVat->simHandle);
		}
		// 잡은 대상이 slicedLimeVat 이라면
		else if(GrabbedActorRight==slicedLimeVat&&slicedLimeVat!=nullptr)
		{
			isGrabbingSlicedLimeVatRight = true;
			GetWorld()->GetTimerManager().ClearTimer(slicedLimeVat->simHandle);

		}
	// 잡은 대상이 halfSlicedOrangeVat 이라면
	else if(GrabbedActorRight==halfSlicedOrangeVat&&halfSlicedOrangeVat!=nullptr)
	{
		isGrabbingHalfSlicedOrangeVatRight = true;
		GetWorld()->GetTimerManager().ClearTimer(halfSlicedOrangeVat->simHandle);

	}
	// 잡은 대상이 slicedOrangeVat 이라면
	else if(GrabbedActorRight==slicedOrangeVat&&slicedOrangeVat!=nullptr)
	{
		isGrabbingSlicedOrangeVatRight = true;
		GetWorld()->GetTimerManager().ClearTimer(slicedOrangeVat->simHandle);

	}
	// 잡은 대상이 oliveVat 이라면
	else if(GrabbedActorRight==oliveVat&&oliveVat!=nullptr)
	{
		isGrabbingOliveVatRight = true;
		GetWorld()->GetTimerManager().ClearTimer(oliveVat->simHandle);

	}
	// 잡은 대상이 iceCubeVat 이라면
	else if(GrabbedActorRight==iceCubeVat&&iceCubeVat!=nullptr)
	{
		isGrabbingIceCubeVatRight = true;
		GetWorld()->GetTimerManager().ClearTimer(iceCubeVat->simHandle);

	}
	
	isDropped=false;
}

void APlayerCharacter::CheckGrabbedObjectLeft()
{
	UGameplayStatics::PlaySound2D(GetWorld(), grabSound, 1.2, 1, 0);

		// Haptic Feedback
		if (PC)
		{
			PC->PlayHapticEffect(HF_GrabObjectLeft, EControllerHand::Left);
		}


	TArray<AActor*> LeftGrippedActors;
	LeftMotionController->GetGrippedActors(LeftGrippedActors);
	for(int i=0; i<LeftGrippedActors.Num(); ++i)
	{
		GrabbedActorLeft = LeftGrippedActors[i];
	}

	// Left Grabbed Actor Casting
	huchuTongL=Cast<AHuchuTong>(GrabbedActorLeft);
	bottleL = Cast<ABottleBase>(GrabbedActorLeft);
	tabletL = Cast<ATablet>(GrabbedActorLeft);
	coasterL=Cast<ACoaster>(GrabbedActorLeft);
	cupL=Cast<ACupBase>(GrabbedActorLeft);
	barSpoonL=Cast<ABarSpoon>(GrabbedActorLeft);
	strainerL=Cast<AStrainer>(GrabbedActorLeft);
	shakerStrainerL=Cast<AShakerStrainer>(GrabbedActorLeft);
	shakerLidL=Cast<AShakerLid>(GrabbedActorLeft);
	shakerL=Cast<AShaker>(GrabbedActorLeft);
	mixingGlassL=Cast<AMixingGlass>(GrabbedActorLeft);
	drinkCanL = Cast<ADrinkCan>(GrabbedActorLeft);
	slicedLimeVatL = Cast<ASlicedLimeVat>(GrabbedActorLeft);
	halfSlicedLimeVatL = Cast<AHalfSlicedLimeVat>(GrabbedActorLeft);
	slicedOrangeVatL = Cast<ASlicedOrangeVat>(GrabbedActorLeft);
	halfSlicedOrangeVatL = Cast<AHalfSlicedOrangeVat>(GrabbedActorLeft);
	oliveVatL = Cast<AOliveVat>(GrabbedActorLeft);
	iceCubeVatL = Cast<AIceCubeVat>(GrabbedActorLeft);
	
	// 잡은 대상이 Tongs라면
	if(GrabbedActorLeft==huchuTongL&&huchuTongL!=nullptr)
	{
		isGrabbingTongsLeft=true;
		GetWorld()->GetTimerManager().ClearTimer(huchuTongL->simHandle);

	}
	// 잡은 대상이 Bottle 이라면
	else if(GrabbedActorLeft == bottleL&&bottleL!=nullptr)
	{
		isGrabbingBottleLeft = true;
		GetWorld()->GetTimerManager().ClearTimer(bottleL->simHandle);
		bottleL->SetActorTickEnabled(true);
		bottleL->isGrabbingBottle=true;
		bottleL->isGBL=true;
	}
	// 잡은 대상이 Tablet 이라면
	else if(GrabbedActorLeft==tabletL&&tabletL!=nullptr)
	{
		isGrabbingTabletLeft=true;
		GetWorld()->GetTimerManager().ClearTimer(tabletL->simHandle);
		tabletL->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		tabletL->VRGripInterfaceSettings.bSimulateOnDrop=true;
		//widgetInteractionComp->bShowDebug=true;
		//widgetInteractionCompLeft->bShowDebug=false;
	}		
	// 잡은 대상이 Coaster이라면
	else if(GrabbedActorLeft==coasterL&&coasterL!=nullptr)
	{
		isGrabbingCoasterLeft=true;
		GetWorld()->GetTimerManager().ClearTimer(coasterL->simHandle);
		coasterL->boxComp->SetCollisionProfileName(FName("CoasterAfterGrab"));

	}
	// 잡은 대상이 Shaker이라면
	else if(GrabbedActorLeft==shakerL&&shakerL!=nullptr)
	{
		isGrabbingShakerLeft=true;
		shakerL->isGSL=true;
		shakerL->SetActorTickEnabled(true);
		shakerL->isGrabbingShaker=true;
		shakerL->VRGripInterfaceSettings.bDenyGripping=true;
		
	}
	// 잡은 대상이 Cup이라면
	else if(GrabbedActorLeft==cupL&&cupL!=nullptr)
	{
		isGrabbingCupLeft=true;
		GetWorld()->GetTimerManager().ClearTimer(cupL->simHandle);
		cupL->SetActorTickEnabled(true);
		cupL->isCupTickActivated=true;
	}
	// 잡은 대상이 BarSpoon이라면
	else if(GrabbedActorLeft==barSpoonL&&barSpoonL!=nullptr)
	{
		isGrabbingBarSpoonLeft=true;
		barSpoonL->meshComp->SetCollisionProfileName(FName("SpoonGrabbed"));

	}

	// 잡은 대상이 ShakerLid라면
	else if(GrabbedActorLeft==shakerLidL&&shakerLidL!=nullptr)
	{
		isGrabbingShakerLidLeft=true;
		GetWorld()->GetTimerManager().ClearTimer(shakerLidL->simHandle);
		shakerLidL->SetLidOff();
		shakerLidL->VRGripInterfaceSettings.bSimulateOnDrop=true;
		shakerLidL->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
	// 잡은 대상이 ShakerStrainer 이라면
	else if(GrabbedActorLeft==shakerStrainerL&&shakerStrainerL!=nullptr)
	{
		isGrabbingShakerStrainerLeft=true;
		GetWorld()->GetTimerManager().ClearTimer(shakerStrainerL->simHandle);
		shakerStrainerL->isStrainerAttachable = false;
		shakerStrainerL->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
	// 잡은 대상이 MixingGlass 라면
	else if(GrabbedActorLeft==mixingGlassL&&mixingGlassL!=nullptr)
	{
		isGrabbingMixingGlassLeft=true;
		mixingGlassL->SetActorTickEnabled(true);
		mixingGlassL->isGrabbingMixingGlass=true;
		mixingGlassL->VRGripInterfaceSettings.bDenyGripping=true;
		mixingGlassL->VRGripInterfaceSettings.bSimulateOnDrop=true;
	}
	// 잡은 대상이 Glass Strainer 이라면
	else if(GrabbedActorLeft==strainerL&&strainerL!=nullptr)
	{
		isGrabbingStrainerLeft=true;
		GetWorld()->GetTimerManager().ClearTimer(strainerL->simHandle);
		strainerL->VRGripInterfaceSettings.bSimulateOnDrop=true;
		strainerL->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
 	}
	// 잡은 대상이 DrinkCan 이라면
	else if(GrabbedActorLeft==drinkCanL&&drinkCanL!=nullptr)
	{
		isGrabbingDrinkCanLeft = true;
	}
	// 잡은 대상이 halfSlicedLimeVat 이라면
	else if(GrabbedActorLeft==halfSlicedLimeVatL&&halfSlicedLimeVatL!=nullptr)
	{
		isGrabbingHalfSlicedLimeVatLeft = true;
		GetWorld()->GetTimerManager().ClearTimer(halfSlicedLimeVatL->simHandle);

	}
	// 잡은 대상이 slicedLimeVat 이라면
	else if(GrabbedActorLeft==slicedLimeVatL&&slicedLimeVatL!=nullptr)
	{
		isGrabbingSlicedLimeVatLeft = true;
		GetWorld()->GetTimerManager().ClearTimer(slicedLimeVatL->simHandle);

	}
	// 잡은 대상이 halfSlicedOrangeVat 이라면
	else if(GrabbedActorLeft==halfSlicedOrangeVatL&&halfSlicedOrangeVatL!=nullptr)
	{
		isGrabbingHalfSlicedOrangeVatLeft = true;
		GetWorld()->GetTimerManager().ClearTimer(halfSlicedOrangeVatL->simHandle);

	}
	// 잡은 대상이 slicedOrangeVat 이라면
	else if(GrabbedActorLeft==slicedOrangeVatL&&slicedOrangeVatL!=nullptr)
	{
		isGrabbingSlicedOrangeVatLeft = true;
		GetWorld()->GetTimerManager().ClearTimer(slicedOrangeVatL->simHandle);

	}
	// 잡은 대상이 oliveVat 이라면
	else if(GrabbedActorLeft==oliveVatL&&oliveVatL!=nullptr)
	{
		isGrabbingOliveVatLeft = true;
		GetWorld()->GetTimerManager().ClearTimer(oliveVatL->simHandle);

	}
	// 잡은 대상이 iceCubeVat 이라면
	else if(GrabbedActorLeft==iceCubeVatL&&iceCubeVatL!=nullptr)
	{
		isGrabbingIceCubeVatLeft = true;
		GetWorld()->GetTimerManager().ClearTimer(iceCubeVatL->simHandle);

	}

	isDropped=false;
}

void APlayerCharacter::CheckDroppedObjectRight()
{
	// 오른손에 Tongs를 잡고 있었다면
	if(isGrabbingTongsRight)
	{
		huchuTong->isDropSoundEnabled=true;
		// Tongs에 잡혀 있는 대상이 있었다면
		if(isGrabbingWithTongsRight)
		{
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			auto tongCompRef = huchuTong->tongRight;
			UKismetSystemLibrary::MoveComponentTo(tongCompRef, tongCompRef->GetRelativeLocation(), tongCompRef->GetRelativeRotation()+FRotator(-5, 0, 0), false, false, 0.0, false, EMoveComponentAction::Move, LatentInfo);
			isTongsTickEnabled = true;
			// 1. 잡지않은 상태로 전환
			isGrabbingWithTongsRight = false;
			// 2. 손에서 떼어내기
			GrabbedObjectWithTongsRight->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			// 3. 물리기능 활성화
			GrabbedObjectWithTongsRight->SetSimulatePhysics(true);
			// 4. 충돌기능 활성화
			GrabbedObjectWithTongsRight->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GrabbedObjectWithTongsRight = nullptr;
		}
		huchuTong->SetSimulateAndTickDisable();
		isGrabbingTongsRight=false;
	}
	else if(isGrabbingBottleRight)
	{
		if(bottle!=nullptr)
		{
			bottle->isGrabbingBottle=false;
			bottle->isGBR=false;
			bottle->isDropSoundEnabled=true;
			bottle->SetSimulateAndTickDisable();
		}
		isGrabbingBottleRight=false;
	}
	else if(isGrabbingTabletRight)
	{
		if(tablet!=nullptr)
		{
			tablet->isDropSoundEnabled=true;
			tablet->AttachToTabletStand();
			tablet->SetSimulateAndTickDisable();

		}
		isGrabbingTabletRight=false;
		//widgetInteractionComp->bShowDebug=false;
		//widgetInteractionCompLeft->bShowDebug=false;
	}
	else if(isGrabbingCoasterRight)
	{
		if(coaster!=nullptr)
		{
			coaster->isDropSoundEnabled=true;
			coaster->boxComp->SetCollisionProfileName(FName("CoasterBeforeGrab"));
			coaster->SetSimulateAndTickDisable();

		}
		isGrabbingCoasterRight=false;
	}
	else if(isGrabbingShakerRight)
	{
		if(shaker!=nullptr)
		{
			shaker->isGSR=false;
			shaker->isDropSoundEnabled=true;			
			shaker->VRGripInterfaceSettings.bDenyGripping=false;
			shaker->isGrabbingShaker=false;
		
		}
		isGrabbingShakerRight=false;		
	}
	else if(isGrabbingCupRight)
	{
		if(cup!=nullptr)
		{
			cup->isDropSoundEnabled=true;
			cup->isCupTickActivated=false;
			cup->SetSimulateAndTickDisable();
		}
		isGrabbingCupRight=false;
	}
	else if(isGrabbingBarSpoonRight)
	{
		if(barSpoon!=nullptr)
		{
			barSpoon->isDropSoundEnabled=true;
			barSpoon->meshComp->SetCollisionProfileName(FName("Spoon"));
		}
		isGrabbingBarSpoonRight=false;
	}
	else if(isGrabbingShakerLidRight)
	{
		if(shakerLid!=nullptr)
		{
			shakerLid->isDropSoundEnabled=true;
			shakerLid->AttachToShakerStrainer();
			shakerLid->SetSimulateAndTickDisable();

		}
		isGrabbingShakerLidRight=false;	
	}
	else if(isGrabbingShakerStrainerRight)
	{
		if(shakerStrainer!=nullptr)
		{
			shakerStrainer->isDropSoundEnabled=true;
			shakerStrainer->AttachToShaker();
			shakerStrainer->SetSimulateAndTickDisable();

		}
		isGrabbingShakerStrainerRight=false;
	
	}
	else if(isGrabbingMixingGlassRight)
	{
		if(mixingGlass!=nullptr)
		{
			mixingGlass->isDropSoundEnabled=true;
			mixingGlass->VRGripInterfaceSettings.bDenyGripping=false;
			mixingGlass->isGrabbingMixingGlass=false;
		}
		isGrabbingMixingGlassRight=false;
	}
	else if(isGrabbingStrainerRight)
	{
		if(strainer!=nullptr)
		{
			strainer->isDropSoundEnabled=true;
			strainer->AttachToMixingGlass();
			strainer->SetSimulateAndTickDisable();

		}
		isGrabbingStrainerRight=false;	
	}

	else if(isGrabbingDrinkCanRight&&drinkCan!=nullptr)
	{
		drinkCan->isDropSoundEnabled=true;
	}
	else if(isGrabbingHalfSlicedLimeVatRight&&halfSlicedLimeVat!=nullptr)
	{
		halfSlicedLimeVat->isDropSoundEnabled=true;
		halfSlicedLimeVat->SetSimulateAndTickDisable();

	}
	else if(isGrabbingSlicedLimeVatRight&&slicedLimeVat!=nullptr)
	{
		slicedLimeVat->isDropSoundEnabled=true;
		slicedLimeVat->SetSimulateAndTickDisable();

	}
	else if(isGrabbingHalfSlicedOrangeVatRight&&halfSlicedOrangeVat!=nullptr)
	{
		halfSlicedOrangeVat->isDropSoundEnabled=true;
		halfSlicedOrangeVat->SetSimulateAndTickDisable();

	}
	else if(isGrabbingSlicedOrangeVatRight&&slicedOrangeVat!=nullptr)
	{
		slicedOrangeVat->isDropSoundEnabled=true;
		slicedOrangeVat->SetSimulateAndTickDisable();

	}
	else if(isGrabbingOliveVatRight&&oliveVat!=nullptr)
	{
		oliveVat->isDropSoundEnabled=true;
		oliveVat->SetSimulateAndTickDisable();

	}
	else if(isGrabbingIceCubeVatRight&&iceCubeVat!=nullptr)
	{
		iceCubeVat->isDropSoundEnabled=true;
		iceCubeVat->SetSimulateAndTickDisable();

	}

	isDropped=true;
}

void APlayerCharacter::CheckDroppedObjectLeft()
{
	// 왼손에 Tongs를 잡고 있었다면
	if(isGrabbingTongsLeft)
	{
		huchuTongL->isDropSoundEnabled=true;
		// Tongs에 잡혀 있는 대상이 있었다면
		if(isGrabbingWithTongsLeft)
		{
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			auto tongCompRef = huchuTongL->tongRight;
			UKismetSystemLibrary::MoveComponentTo(tongCompRef, tongCompRef->GetRelativeLocation(), tongCompRef->GetRelativeRotation()+FRotator(-5, 0, 0), false, false, 0.0, false, EMoveComponentAction::Move, LatentInfo);
			isTongsTickEnabledL = true;
			// 1. 잡지않은 상태로 전환
			isGrabbingWithTongsLeft = false;
			// 2. 손에서 떼어내기
			GrabbedObjectWithTongsLeft->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			// 3. 물리기능 활성화
			GrabbedObjectWithTongsLeft->SetSimulatePhysics(true);
			// 4. 충돌기능 활성화
			GrabbedObjectWithTongsLeft->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GrabbedObjectWithTongsLeft = nullptr;
		}
		huchuTongL->SetSimulateAndTickDisable();
		isGrabbingTongsLeft=false;
	}
	else if(isGrabbingBottleLeft&&bottleL!=nullptr)
	{
		bottleL->isDropSoundEnabled=true;
		bottleL->isGBL=false;
		bottleL->isGrabbingBottle=false;
		bottleL->SetSimulateAndTickDisable();
		isGrabbingBottleLeft=false;
	}
	else if(isGrabbingTabletLeft&&tabletL!=nullptr)
	{
		isGrabbingTabletLeft=false;
		tabletL->AttachToTabletStand();
		tabletL->isDropSoundEnabled=true;
		tabletL->SetSimulateAndTickDisable();

		//widgetInteractionComp->bShowDebug=false;
		//widgetInteractionCompLeft->bShowDebug=false;
	}
	else if(isGrabbingCoasterLeft&&coasterL!=nullptr)
	{
		coasterL->isDropSoundEnabled=true;
		coasterL->boxComp->SetCollisionProfileName(FName("CoasterBeforeGrab"));
		coasterL->SetSimulateAndTickDisable();

		isGrabbingCoasterLeft=false;
	}
	else if(isGrabbingBarSpoonLeft&&barSpoonL!=nullptr)
	{
		barSpoonL->isDropSoundEnabled=true;
		barSpoonL->meshComp->SetCollisionProfileName(FName("Spoon"));

	}
	else if(isGrabbingShakerLeft&&shakerL!=nullptr)
	{
		shakerL->isGSL=false;
		shakerL->VRGripInterfaceSettings.bDenyGripping=false;
		shakerL->isDropSoundEnabled=true;
		shakerL->isGrabbingShaker=false;		
		isGrabbingShakerLeft=false;		
	}
	else if(isGrabbingCupLeft&&cupL!=nullptr)
	{
		cupL->isDropSoundEnabled=true;
		cupL->isCupTickActivated=false;
		cupL->SetSimulateAndTickDisable();
		isGrabbingCupLeft=false;
	}
	else if(isGrabbingShakerLidLeft&&shakerLidL!=nullptr)
	{			
		shakerLidL->isDropSoundEnabled=true;
		shakerLidL->AttachToShakerStrainer();
		shakerLidL->SetSimulateAndTickDisable();

		isGrabbingShakerLidLeft=false;	
	}
	else if(isGrabbingShakerStrainerLeft&&shakerStrainerL!=nullptr)
	{	
		shakerStrainerL->isDropSoundEnabled=true;
		shakerStrainerL->AttachToShaker();
		shakerStrainerL->SetSimulateAndTickDisable();

		isGrabbingShakerStrainerLeft=false;
	
	}
	else if(isGrabbingMixingGlassLeft&&mixingGlassL!=nullptr)
	{
		mixingGlassL->VRGripInterfaceSettings.bDenyGripping=false;
		mixingGlassL->isDropSoundEnabled=true;
		mixingGlassL->isGrabbingMixingGlass=false;
		isGrabbingMixingGlassLeft=false;
	}
	else if(isGrabbingStrainerLeft&&strainerL!=nullptr)
	{
		strainerL->isDropSoundEnabled=true;
		strainerL->AttachToMixingGlass();
		strainerL->SetSimulateAndTickDisable();

		isGrabbingStrainerLeft=false;	
	}

	else if(isGrabbingDrinkCanLeft&&drinkCanL!=nullptr)
	{
		drinkCanL->isDropSoundEnabled=true;
	}
	else if(isGrabbingHalfSlicedLimeVatLeft&&halfSlicedLimeVatL!=nullptr)
	{
		halfSlicedLimeVatL->isDropSoundEnabled=true;
		halfSlicedLimeVatL->SetSimulateAndTickDisable();

	}
	else if(isGrabbingSlicedLimeVatLeft&&slicedLimeVatL!=nullptr)
	{
		slicedLimeVatL->isDropSoundEnabled=true;
		slicedLimeVatL->SetSimulateAndTickDisable();

	}
	else if(isGrabbingHalfSlicedOrangeVatLeft&&halfSlicedOrangeVatL!=nullptr)
	{
		halfSlicedOrangeVatL->isDropSoundEnabled=true;
		halfSlicedOrangeVatL->SetSimulateAndTickDisable();

	}
	else if(isGrabbingSlicedOrangeVatLeft&&slicedOrangeVatL!=nullptr)
	{
		slicedOrangeVatL->isDropSoundEnabled=true;
		slicedOrangeVatL->SetSimulateAndTickDisable();

	}
	else if(isGrabbingOliveVatLeft&&oliveVatL!=nullptr)
	{
		oliveVatL->isDropSoundEnabled=true;
		oliveVatL->SetSimulateAndTickDisable();

	}
	else if(isGrabbingIceCubeVatLeft&&iceCubeVatL!=nullptr)
	{
		iceCubeVatL->isDropSoundEnabled=true;
		iceCubeVatL->SetSimulateAndTickDisable();

	}
	
	isDropped=true;

}

void APlayerCharacter::DestroyDroppedObjectRight()
{
	// 오른손에 Tongs를 잡고 있었다면
	if(isGrabbingTongsRight)
	{
		huchuTong->isDropSoundEnabled=true;
		// Tongs에 잡혀 있는 대상이 있었다면
		if(isGrabbingWithTongsRight)
		{
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			auto tongCompRef = huchuTong->tongRight;
			UKismetSystemLibrary::MoveComponentTo(tongCompRef, tongCompRef->GetRelativeLocation(), tongCompRef->GetRelativeRotation()+FRotator(-5, 0, 0), false, false, 0.0, false, EMoveComponentAction::Move, LatentInfo);
			isTongsTickEnabled = true;
			// 1. 잡지않은 상태로 전환
			isGrabbingWithTongsRight = false;
			// 2. 손에서 떼어내기
			GrabbedObjectWithTongsRight->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			// 3. 물리기능 활성화
			GrabbedObjectWithTongsRight->SetSimulatePhysics(true);
			// 4. 충돌기능 활성화
			GrabbedObjectWithTongsRight->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GrabbedObjectWithTongsRight = nullptr;
		}
		huchuTong->SetSimulateAndTickDisable();
		isGrabbingTongsRight=false;
		
		if(huchuTong)
		huchuTong->Destroy();
	}
	else if(isGrabbingBottleRight)
	{
		if(bottle!=nullptr)
		{
			bottle->isGrabbingBottle=false;
			bottle->isGBR=false;
			bottle->isDropSoundEnabled=true;
			bottle->SetSimulateAndTickDisable();
		}
		isGrabbingBottleRight=false;

		if(bottle)
			bottle->Destroy();
	}
	else if(isGrabbingTabletRight)
	{
		if(tablet!=nullptr)
		{
			tablet->isDropSoundEnabled=true;
			tablet->AttachToTabletStand();
			tablet->SetSimulateAndTickDisable();

		}
		isGrabbingTabletRight=false;
		//widgetInteractionComp->bShowDebug=false;
		//widgetInteractionCompLeft->bShowDebug=false;
		if(tablet)
			tablet->Destroy();
	}
	else if(isGrabbingCoasterRight)
	{
		if(coaster!=nullptr)
		{
			coaster->isDropSoundEnabled=true;
			coaster->boxComp->SetCollisionProfileName(FName("CoasterBeforeGrab"));
			coaster->SetSimulateAndTickDisable();

		}
		isGrabbingCoasterRight=false;
		if(coaster)
			coaster->Destroy();
	}
	else if(isGrabbingShakerRight)
	{
		if(shaker!=nullptr)
		{
			shaker->isGSR=false;
			shaker->isDropSoundEnabled=true;			
			shaker->VRGripInterfaceSettings.bDenyGripping=false;
			shaker->isGrabbingShaker=false;
		
		}
		isGrabbingShakerRight=false;
		if(shaker)
			shaker->Destroy();
	}
	else if(isGrabbingCupRight)
	{
		if(cup!=nullptr)
		{
			cup->isDropSoundEnabled=true;
			cup->isCupTickActivated=false;
			cup->SetSimulateAndTickDisable();
		}
		isGrabbingCupRight=false;
		if(cup)
			cup->Destroy();
	}
	else if(isGrabbingBarSpoonRight)
	{
		if(barSpoon!=nullptr)
		{
			barSpoon->isDropSoundEnabled=true;
			barSpoon->meshComp->SetCollisionProfileName(FName("Spoon"));
		}
		isGrabbingBarSpoonRight=false;
		if(barSpoon)
			barSpoon->Destroy();
	}
	else if(isGrabbingShakerLidRight)
	{
		if(shakerLid!=nullptr)
		{
			shakerLid->isDropSoundEnabled=true;
			shakerLid->AttachToShakerStrainer();
			shakerLid->SetSimulateAndTickDisable();

		}
		isGrabbingShakerLidRight=false;
		if(shakerLid)
			shakerLid->Destroy();
	}
	else if(isGrabbingShakerStrainerRight)
	{
		if(shakerStrainer!=nullptr)
		{
			shakerStrainer->isDropSoundEnabled=true;
			shakerStrainer->AttachToShaker();
			shakerStrainer->SetSimulateAndTickDisable();

		}
		isGrabbingShakerStrainerRight=false;
		if(shakerStrainer)
			shakerStrainer->Destroy();
	
	}
	else if(isGrabbingMixingGlassRight)
	{
		if(mixingGlass!=nullptr)
		{
			mixingGlass->isDropSoundEnabled=true;
			mixingGlass->VRGripInterfaceSettings.bDenyGripping=false;
			mixingGlass->isGrabbingMixingGlass=false;
		}
		isGrabbingMixingGlassRight=false;
		if(mixingGlass)
			mixingGlass->Destroy();
	}
	else if(isGrabbingStrainerRight)
	{
		if(strainer!=nullptr)
		{
			strainer->isDropSoundEnabled=true;
			strainer->AttachToMixingGlass();
			strainer->SetSimulateAndTickDisable();

		}
		isGrabbingStrainerRight=false;
		if(strainer)
			strainer->Destroy();
	}

	else if(isGrabbingDrinkCanRight&&drinkCan!=nullptr)
	{
		drinkCan->isDropSoundEnabled=true;
		if(drinkCan)
			drinkCan->Destroy();
	}
	else if(isGrabbingHalfSlicedLimeVatRight&&halfSlicedLimeVat!=nullptr)
	{
		halfSlicedLimeVat->isDropSoundEnabled=true;
		halfSlicedLimeVat->SetSimulateAndTickDisable();
		if(halfSlicedLimeVat)
			halfSlicedLimeVat->Destroy();

	}
	else if(isGrabbingSlicedLimeVatRight&&slicedLimeVat!=nullptr)
	{
		slicedLimeVat->isDropSoundEnabled=true;
		slicedLimeVat->SetSimulateAndTickDisable();
		if(slicedLimeVat)
			slicedLimeVat->Destroy();

	}
	else if(isGrabbingHalfSlicedOrangeVatRight&&halfSlicedOrangeVat!=nullptr)
	{
		halfSlicedOrangeVat->isDropSoundEnabled=true;
		halfSlicedOrangeVat->SetSimulateAndTickDisable();
		if(halfSlicedOrangeVat)
			halfSlicedOrangeVat->Destroy();

	}
	else if(isGrabbingSlicedOrangeVatRight&&slicedOrangeVat!=nullptr)
	{
		slicedOrangeVat->isDropSoundEnabled=true;
		slicedOrangeVat->SetSimulateAndTickDisable();
		if(slicedOrangeVat)
			slicedOrangeVat->Destroy();

	}
	else if(isGrabbingOliveVatRight&&oliveVat!=nullptr)
	{
		oliveVat->isDropSoundEnabled=true;
		oliveVat->SetSimulateAndTickDisable();
		if(oliveVat)
			oliveVat->Destroy();
	}
	else if(isGrabbingIceCubeVatRight&&iceCubeVat!=nullptr)
	{
		iceCubeVat->isDropSoundEnabled=true;
		iceCubeVat->SetSimulateAndTickDisable();
		if(iceCubeVat)
			iceCubeVat->Destroy();
	}

	isDropped=true;
}

void APlayerCharacter::DestroyDroppedObjectLeft()
{
	// 왼손에 Tongs를 잡고 있었다면
	if(isGrabbingTongsLeft)
	{
		huchuTongL->isDropSoundEnabled=true;
		// Tongs에 잡혀 있는 대상이 있었다면
		if(isGrabbingWithTongsLeft)
		{
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			auto tongCompRef = huchuTongL->tongRight;
			UKismetSystemLibrary::MoveComponentTo(tongCompRef, tongCompRef->GetRelativeLocation(), tongCompRef->GetRelativeRotation()+FRotator(-5, 0, 0), false, false, 0.0, false, EMoveComponentAction::Move, LatentInfo);
			isTongsTickEnabledL = true;
			// 1. 잡지않은 상태로 전환
			isGrabbingWithTongsLeft = false;
			// 2. 손에서 떼어내기
			GrabbedObjectWithTongsLeft->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			// 3. 물리기능 활성화
			GrabbedObjectWithTongsLeft->SetSimulatePhysics(true);
			// 4. 충돌기능 활성화
			GrabbedObjectWithTongsLeft->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GrabbedObjectWithTongsLeft = nullptr;
		}
		huchuTongL->SetSimulateAndTickDisable();
		isGrabbingTongsLeft=false;
		if(huchuTongL)
			huchuTongL->Destroy();
	}
	else if(isGrabbingBottleLeft&&bottleL!=nullptr)
	{
		bottleL->isDropSoundEnabled=true;
		bottleL->isGBL=false;
		bottleL->isGrabbingBottle=false;
		bottleL->SetSimulateAndTickDisable();
		isGrabbingBottleLeft=false;
		if(bottleL)
			bottleL->Destroy();
	}
	else if(isGrabbingTabletLeft&&tabletL!=nullptr)
	{
		isGrabbingTabletLeft=false;
		tabletL->AttachToTabletStand();
		tabletL->isDropSoundEnabled=true;
		tabletL->SetSimulateAndTickDisable();

		//widgetInteractionComp->bShowDebug=false;
		//widgetInteractionCompLeft->bShowDebug=false;
		if(tabletL)
			tabletL->Destroy();
	}
	else if(isGrabbingCoasterLeft&&coasterL!=nullptr)
	{
		coasterL->isDropSoundEnabled=true;
		coasterL->boxComp->SetCollisionProfileName(FName("CoasterBeforeGrab"));
		coasterL->SetSimulateAndTickDisable();

		isGrabbingCoasterLeft=false;
		if(coasterL)
			coasterL->Destroy();
	}
	else if(isGrabbingBarSpoonLeft&&barSpoonL!=nullptr)
	{
		barSpoonL->isDropSoundEnabled=true;
		barSpoonL->meshComp->SetCollisionProfileName(FName("Spoon"));
		if(barSpoonL)
			barSpoonL->Destroy();

	}
	else if(isGrabbingShakerLeft&&shakerL!=nullptr)
	{
		shakerL->isGSL=false;
		shakerL->VRGripInterfaceSettings.bDenyGripping=false;
		shakerL->isDropSoundEnabled=true;
		shakerL->isGrabbingShaker=false;		
		isGrabbingShakerLeft=false;
		if(shakerL)
			shakerL->Destroy();
	}
	else if(isGrabbingCupLeft&&cupL!=nullptr)
	{
		cupL->isDropSoundEnabled=true;
		cupL->isCupTickActivated=false;
		cupL->SetSimulateAndTickDisable();
		isGrabbingCupLeft=false;
		if(cupL)
			cupL->Destroy();
	}
	else if(isGrabbingShakerLidLeft&&shakerLidL!=nullptr)
	{			
		shakerLidL->isDropSoundEnabled=true;
		shakerLidL->AttachToShakerStrainer();
		shakerLidL->SetSimulateAndTickDisable();

		isGrabbingShakerLidLeft=false;
		if(shakerLidL)
			shakerLidL->Destroy();
	}
	else if(isGrabbingShakerStrainerLeft&&shakerStrainerL!=nullptr)
	{	
		shakerStrainerL->isDropSoundEnabled=true;
		shakerStrainerL->AttachToShaker();
		shakerStrainerL->SetSimulateAndTickDisable();

		isGrabbingShakerStrainerLeft=false;
		if(shakerStrainerL)
			shakerStrainerL->Destroy();
	
	}
	else if(isGrabbingMixingGlassLeft&&mixingGlassL!=nullptr)
	{
		mixingGlassL->VRGripInterfaceSettings.bDenyGripping=false;
		mixingGlassL->isDropSoundEnabled=true;
		mixingGlassL->isGrabbingMixingGlass=false;
		isGrabbingMixingGlassLeft=false;
		if(mixingGlassL)
			mixingGlassL->Destroy();
	}
	else if(isGrabbingStrainerLeft&&strainerL!=nullptr)
	{
		strainerL->isDropSoundEnabled=true;
		strainerL->AttachToMixingGlass();
		strainerL->SetSimulateAndTickDisable();

		isGrabbingStrainerLeft=false;
		if(strainerL)
			strainerL->Destroy();
			
	}

	else if(isGrabbingDrinkCanLeft&&drinkCanL!=nullptr)
	{
		drinkCanL->isDropSoundEnabled=true;
		if(drinkCanL)
			drinkCanL->Destroy();
	}
	else if(isGrabbingHalfSlicedLimeVatLeft&&halfSlicedLimeVatL!=nullptr)
	{
		halfSlicedLimeVatL->isDropSoundEnabled=true;
		halfSlicedLimeVatL->SetSimulateAndTickDisable();
		if(halfSlicedLimeVatL)
			halfSlicedLimeVatL->Destroy();

	}
	else if(isGrabbingSlicedLimeVatLeft&&slicedLimeVatL!=nullptr)
	{
		slicedLimeVatL->isDropSoundEnabled=true;
		slicedLimeVatL->SetSimulateAndTickDisable();
		if(slicedLimeVatL)
			slicedLimeVatL->Destroy();

	}
	else if(isGrabbingHalfSlicedOrangeVatLeft&&halfSlicedOrangeVatL!=nullptr)
	{
		halfSlicedOrangeVatL->isDropSoundEnabled=true;
		halfSlicedOrangeVatL->SetSimulateAndTickDisable();
		if(halfSlicedOrangeVatL)
			halfSlicedOrangeVatL->Destroy();

	}
	else if(isGrabbingSlicedOrangeVatLeft&&slicedOrangeVatL!=nullptr)
	{
		slicedOrangeVatL->isDropSoundEnabled=true;
		slicedOrangeVatL->SetSimulateAndTickDisable();
		if(slicedOrangeVatL)
			slicedOrangeVatL->Destroy();

	}
	else if(isGrabbingOliveVatLeft&&oliveVatL!=nullptr)
	{
		oliveVatL->isDropSoundEnabled=true;
		oliveVatL->SetSimulateAndTickDisable();
		if(oliveVatL)
			oliveVatL->Destroy();

	}
	else if(isGrabbingIceCubeVatLeft&&iceCubeVatL!=nullptr)
	{
		iceCubeVatL->isDropSoundEnabled=true;
		iceCubeVatL->SetSimulateAndTickDisable();
		if(iceCubeVatL)
			iceCubeVatL->Destroy();

	}
	
	isDropped=true;

}

void APlayerCharacter::FireRight()
{	
	if(widgetInteractionComp)
	{
		//UI에 이벤트를 전달하고 싶다.
		widgetInteractionComp->PressPointerKey(FKey(FName("LeftMouseButton")));
		bool isCursorOn = widgetInteractionComp->IsOverHitTestVisibleWidget();
		if(isCursorOn)
		{
			UGameplayStatics::PlaySound2D(GetWorld(),TriggerPressedSound, 1, 1, 0);
		}
		//widgetInteractionComp->ReleasePointerKey(FKey(FName("LeftMouseButton")));

	}
	// 오른손에 Tongs를 쥐고 있다면
	if(isGrabbingTongsRight)
	{
		// 집게를 든채로 Vat에서 Trigger 되었는지 판단하는 OverlapMulti
		// 중심점
		FVector CenterTong = huchuTong->tongRight->GetSocketLocation(FName("TongAttach"));
		// 충돌체크(구충돌)
		// 충돌한 물체를 기억할 배열
		TArray<FOverlapResult> VatHitObj;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		params.AddIgnoredActor(huchuTong);
		bool bHitVat = GetWorld()->OverlapMultiByChannel(VatHitObj, CenterTong, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(5), params);
		if (bHitVat == false)
		{
			return;
		}
		int32 ClosestVat = 0;
		for (int i = 0; i < VatHitObj.Num(); ++i)
		{
			isVatCastedRight = true;
			float ClosestDist = FVector::Dist(VatHitObj[ClosestVat].GetActor()->GetActorLocation(), CenterTong);
			float NextDist = FVector::Dist(VatHitObj[i].GetActor()->GetActorLocation(), CenterTong);

			// 만약 이번 대상이 현재 대상보다 가깝다면,
			if (NextDist < ClosestDist)
			{
				// 가장 가까운 대상으로 변경하기
				ClosestVat = i;
			}				
		}
		if(isVatCastedRight)
		{
			halfSlicedLimeVat=Cast<AHalfSlicedLimeVat>(VatHitObj[ClosestVat].GetActor());
			slicedLimeVat=Cast<ASlicedLimeVat>(VatHitObj[ClosestVat].GetActor());
			iceCubeVat=Cast<AIceCubeVat>(VatHitObj[ClosestVat].GetActor());
			oliveVat=Cast<AOliveVat>(VatHitObj[ClosestVat].GetActor());
			halfSlicedOrangeVat = Cast<AHalfSlicedOrangeVat>(VatHitObj[ClosestVat].GetActor());
			slicedOrangeVat = Cast<ASlicedOrangeVat>(VatHitObj[ClosestVat].GetActor());
			if(halfSlicedLimeVat)
			{
				// Haptic Feedback
				if(PC)
				{
					PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Right);			
				}
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto socketLoc = huchuTong->tongRight->GetSocketLocation(FName("LimeSocket"));
				auto socketRot = huchuTong->tongRight-> GetSocketRotation(FName("LimeSocket"));
				GetWorld()->SpawnActor<AHalfSlicedLime>(halfSlicedLimeFac, socketLoc, socketRot, param);
			}
			else if(slicedLimeVat)
			{
				// Haptic Feedback
				if(PC)
				{
					PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Right);			
				}
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto socketLoc = huchuTong->tongRight->GetSocketLocation(FName("LimeSocket"));
				auto socketRot = huchuTong->tongRight-> GetSocketRotation(FName("LimeSocket"));
				GetWorld()->SpawnActor<ASlicedLime>(slicedLimeFac, socketLoc, socketRot, param);
			}
			else if(iceCubeVat)
			{
				// Haptic Feedback
				if(PC)
				{
					PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Right);			
				}
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto socketLoc = huchuTong->tongRight->GetSocketLocation(FName("TongGrabSizeSocket"));
				auto socketRot = huchuTong->tongRight-> GetSocketRotation(FName("TongGrabSizeSocket"));
				GetWorld()->SpawnActor<AIceCube>(iceCubeFac, socketLoc, socketRot, param);
			}
			else if(oliveVat)
			{
				// Haptic Feedback
				if(PC)
				{
					PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Right);			
				}
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto socketLoc = huchuTong->tongRight->GetSocketLocation(FName("OliveSocket"));
				auto socketRot = huchuTong->tongRight-> GetSocketRotation(FName("OliveSocket"));
				GetWorld()->SpawnActor<AOlivePick>(oliveFac, socketLoc, socketRot, param);
			}
			else if(halfSlicedOrangeVat)
			{
				// Haptic Feedback
				if(PC)
				{
					PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Right);			
				}
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto socketLoc = huchuTong->tongRight->GetSocketLocation(FName("LimeSocket"));
				auto socketRot = huchuTong->tongRight-> GetSocketRotation(FName("LimeSocket"));
				GetWorld()->SpawnActor<AHalfSlicedOrange>(halfSlicedOrangeFac, socketLoc, socketRot, param);
			}
			else if(slicedOrangeVat)
			{
				// Haptic Feedback
				if(PC)
				{
					PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Right);			
				}
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto socketLoc = huchuTong->tongRight->GetSocketLocation(FName("LimeSocket"));
				auto socketRot = huchuTong->tongRight-> GetSocketRotation(FName("LimeSocket"));
				GetWorld()->SpawnActor<ASlicedOrange>(slicedOrangeFac, socketLoc, socketRot, param);
			}
			isVatCastedRight=false;
		}
		// 집게에 집는 대상 오브젝트가 오버랩되었는지 판단하는 OverlapMulti
		// 중심점
		FVector Center = huchuTong->tongRight->GetSocketLocation(FName("TongAttach"));
		// 충돌체크(구충돌)
		// 충돌한 물체를 기억할 배열
		TArray<FOverlapResult> HitObj;
		bool bHit = GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(TongsGrabRange), params);
		if (bHit == false)
		{
			return;
		}
		// 가장 가까운 물체를 잡도록 하자
		// 가장 가까운 물체 인덱스
		int32 Closest = 0;
		for (int i = 0; i < HitObj.Num(); ++i)
		{
			// 1. 물리기능이 활성화 되어 있는 대상만 판단
			if (HitObj[i].GetComponent()->IsSimulatingPhysics() == false)
			{
				continue;
			}
			// 잡기에 성공했다
			isGrabbingWithTongsRight = true;
			// 2.. 현재 손과 가장 가까운 대상과 이번에 검출할 대상과 더 가까운 대상이 있다면		
			// 필요속성 : 현재 가장 가까운 대상과 손과의 거리
			float ClosestDist = FVector::Dist(HitObj[Closest].GetActor()->GetActorLocation(), Center);
			// 필요속성 : 이번에 검출할 대상과 손과의 거리
			float NextDist = FVector::Dist(HitObj[i].GetActor()->GetActorLocation(), Center);

			// 3. 만약 이번 대상이 현재 대상보다 가깝다면,
			if (NextDist < ClosestDist)
			{

				// 가장 가까운 대상으로 변경하기
				Closest = i;
			}
		}		
		// ice cube 잡기에 성공했다면
		if (isGrabbingWithTongsRight&&HitObj[Closest].GetComponent()->IsSimulatingPhysics() == true)
		{
			GrabbedActorWithTongsRight=HitObj[Closest].GetComponent()->GetAttachmentRootActor();
			GrabbedObjectWithTongsRight = HitObj[Closest].GetComponent();
			iceCube=Cast<AIceCube>(GrabbedActorWithTongsRight);
			slicedLime=Cast<ASlicedLime>(GrabbedActorWithTongsRight);
			halfSlicedLime=Cast<AHalfSlicedLime>(GrabbedActorWithTongsRight);
			olivePick=Cast<AOlivePick>(GrabbedActorWithTongsRight);
			slicedOrange = Cast<ASlicedOrange>(GrabbedActorWithTongsRight);
			halfSlicedOrange=Cast<AHalfSlicedOrange>(GrabbedActorWithTongsRight);
			
			if(GrabbedActorWithTongsRight==iceCube&&iceCube!=nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), TongGrabSound, 1, 1, 0);
				isGrabbingIceWithTongsRight=true;
				GrabbedObjectWithTongsRight->SetSimulatePhysics(false);
				GrabbedObjectWithTongsRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);				
				GrabbedObjectWithTongsRight->AttachToComponent(huchuTong->tongRight,FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("TongGrabSizeSocket"));
			}
			else if(GrabbedActorWithTongsRight==slicedLime&&slicedLime!=nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), TongGrabSound, 1, 1, 0);
				isGrabbingLimeWithTongsRight=true;
				GrabbedObjectWithTongsRight->SetSimulatePhysics(false);
				GrabbedObjectWithTongsRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);				
				GrabbedObjectWithTongsRight->AttachToComponent(huchuTong->tongRight,FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LimeSocket"));
			}
			else if(GrabbedActorWithTongsRight==halfSlicedLime&&halfSlicedLime!=nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), TongGrabSound, 1, 1, 0);
				isGrabbingLimeWithTongsRight=true;
				GrabbedObjectWithTongsRight->SetSimulatePhysics(false);
				GrabbedObjectWithTongsRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);				
				GrabbedObjectWithTongsRight->AttachToComponent(huchuTong->tongRight,FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LimeSocket"));
			}
			else if(GrabbedActorWithTongsRight==olivePick&&olivePick!=nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), TongGrabSound, 1, 1, 0);
				isGrabbingOliveWithTongsRight=true;
				GrabbedObjectWithTongsRight->SetSimulatePhysics(false);
				GrabbedObjectWithTongsRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);				
				GrabbedObjectWithTongsRight->AttachToComponent(huchuTong->tongRight,FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("OliveSocket"));
			}
			else if(GrabbedActorWithTongsRight==slicedOrange&&slicedOrange!=nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), TongGrabSound, 1, 1, 0);
				isGrabbingOrangeWithTongsRight=true;
				GrabbedObjectWithTongsRight->SetSimulatePhysics(false);
				GrabbedObjectWithTongsRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);				
				GrabbedObjectWithTongsRight->AttachToComponent(huchuTong->tongRight,FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LimeSocket"));
			}
			else if(GrabbedActorWithTongsRight==halfSlicedOrange&&halfSlicedOrange!=nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), TongGrabSound, 1, 1, 0);
				isGrabbingOrangeWithTongsRight=true;
				GrabbedObjectWithTongsRight->SetSimulatePhysics(false);
				GrabbedObjectWithTongsRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);				
				GrabbedObjectWithTongsRight->AttachToComponent(huchuTong->tongRight,FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LimeSocket"));
			}
		}		
			auto tongCompRef = huchuTong->tongRight;
			auto tongLoc =  huchuTong->tongRight->GetSocketLocation(FName("TongGrabSizeSocket"));
			auto tongRightVector = huchuTong->GetActorForwardVector();
			FCollisionQueryParams params1;
			params1.AddIgnoredActor(huchuTong);  
			params1.AddIgnoredActor(this);				
			FHitResult leftTrace;
			FHitResult rightTrace;
			// Tongs 양쪽에서 LineTrace롤 통해 Grab할 대상의 크기를 측정한다.
			bool bHitR = GetWorld()->LineTraceSingleByChannel(rightTrace,tongLoc+tongRightVector*25.0f, tongLoc+tongRightVector*-25.0f, ECC_Visibility,params1);
			bool bHitL = GetWorld()->LineTraceSingleByChannel(leftTrace,tongLoc+tongRightVector*-25.0f, tongLoc+tongRightVector*25.0f, ECC_Visibility,params1);
			// LineTrace가 양쪽 모두 적중했다면
			if(bHitL&&bHitR)
			{
				if(isGrabbingIceWithTongsRight)
				{
					isTongsTickEnabled = false;
					// Left Impact Point와 Right Impact Point 사이의 간격을 도출한다
					//grabbingObjectSize = FVector::Dist(leftTrace.ImpactPoint, rightTrace.ImpactPoint);
					// grabbingObjectSize에 따라서 Tongs가 다물어질 정도를 결정한다.
					tongCompRef->SetRelativeRotation(FRotator(5, 0, 0));
				}
				else if(isGrabbingLimeWithTongsRight)
				{
					isTongsTickEnabled = false;
					tongCompRef->SetRelativeRotation(FRotator(13, 0, 0));
				}
				else if(isGrabbingOliveWithTongsRight)
				{
					isTongsTickEnabled = false;
					tongCompRef->SetRelativeRotation(FRotator(12, 0, 0));
				}
				else if(isGrabbingOrangeWithTongsRight)
				{
					isTongsTickEnabled = false;
					tongCompRef->SetRelativeRotation(FRotator(13, 0, 0));
				}
			}
			// LineTrace가 적중하지 않았다면 -> 허공이라면
			else
			{
				// Oculus Trigger Input Value에 따른 Tongs Rotation 제어 Tick 활성화
				isTongsTickEnabled=true;
			}

	}	
	if(isGrabbingCupRight&&cup!=nullptr)
	{
		cup->SetCupEmpty();
	}
}

void APlayerCharacter::FireLeft()
{
	if(widgetInteractionCompLeft)
	{
		widgetInteractionCompLeft->PressPointerKey(FKey(FName("LeftMouseButton")));
		//widgetInteractionCompLeft->ReleasePointerKey(FKey(FName("LeftMouseButton")));
		bool isCursorOn = widgetInteractionComp->IsOverHitTestVisibleWidget();
		if(isCursorOn)
		{
			UGameplayStatics::PlaySound2D(GetWorld(),TriggerPressedSound, 1, 1, 0);
		}
	}
	// 왼손에 Tongs를 쥐고 있다면
	if(isGrabbingTongsLeft)
	{
		// 집게를 든채로 Vat에서 Trigger 되었는지 판단하는 OverlapMulti
		// 중심점
		FVector CenterTong = huchuTongL->tongRight->GetSocketLocation(FName("TongAttach"));
		// 충돌체크(구충돌)
		// 충돌한 물체를 기억할 배열
		TArray<FOverlapResult> VatHitObj;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		params.AddIgnoredActor(huchuTongL);
		bool bHitVat = GetWorld()->OverlapMultiByChannel(VatHitObj, CenterTong, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(5), params);
		if (bHitVat == false)
		{
			return;
		}		
		int32 ClosestVat = 0;
		for (int i = 0; i < VatHitObj.Num(); ++i)
		{
			isVatCastedLeft = true;
			float ClosestDist = FVector::Dist(VatHitObj[ClosestVat].GetActor()->GetActorLocation(), CenterTong);
			float NextDist = FVector::Dist(VatHitObj[i].GetActor()->GetActorLocation(), CenterTong);

			// 만약 이번 대상이 현재 대상보다 가깝다면,
			if (NextDist < ClosestDist)
			{
				// 가장 가까운 대상으로 변경하기
				ClosestVat = i;
			}				
		}
		if(isVatCastedLeft)
		{
			halfSlicedLimeVat=Cast<AHalfSlicedLimeVat>(VatHitObj[ClosestVat].GetActor());
			slicedLimeVat=Cast<ASlicedLimeVat>(VatHitObj[ClosestVat].GetActor());
			iceCubeVat=Cast<AIceCubeVat>(VatHitObj[ClosestVat].GetActor());
			oliveVat=Cast<AOliveVat>(VatHitObj[ClosestVat].GetActor());
			halfSlicedOrangeVat=Cast<AHalfSlicedOrangeVat>(VatHitObj[ClosestVat].GetActor());
			slicedOrangeVat=Cast<ASlicedOrangeVat>(VatHitObj[ClosestVat].GetActor());
			if(halfSlicedLimeVat)
			{
				// Haptic Feedback
				if(PC)
				{
					PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Left);			
				}
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto socketLoc = huchuTongL->tongRight->GetSocketLocation(FName("LimeSocket"));
				auto socketRot = huchuTongL->tongRight-> GetSocketRotation(FName("LimeSocket"));
				GetWorld()->SpawnActor<AHalfSlicedLime>(halfSlicedLimeFac, socketLoc, socketRot, param);
			}
			else if(slicedLimeVat)
			{
				// Haptic Feedback
				if(PC)
				{
					PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Left);			
				}
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto socketLoc = huchuTongL->tongRight->GetSocketLocation(FName("LimeSocket"));
				auto socketRot = huchuTongL->tongRight-> GetSocketRotation(FName("LimeSocket"));
				GetWorld()->SpawnActor<ASlicedLime>(slicedLimeFac, socketLoc, socketRot, param);
			}
			else if(iceCubeVat)
			{
				// Haptic Feedback
				if(PC)
				{
					PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Left);			
				}
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto socketLoc = huchuTongL->tongRight->GetSocketLocation(FName("TongGrabSizeSocket"));
				auto socketRot = huchuTongL->tongRight-> GetSocketRotation(FName("TongGrabSizeSocket"));
				GetWorld()->SpawnActor<AIceCube>(iceCubeFac, socketLoc, socketRot, param);
			}
			else if(oliveVat)
			{
				// Haptic Feedback
				if(PC)
				{
					PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Left);			
				}
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto socketLoc = huchuTongL->tongRight->GetSocketLocation(FName("OliveSocket"));
				auto socketRot = huchuTongL->tongRight-> GetSocketRotation(FName("OliveSocket"));
				GetWorld()->SpawnActor<AOlivePick>(oliveFac, socketLoc, socketRot, param);
			}
			else if(slicedOrangeVat)
			{
				// Haptic Feedback
				if(PC)
				{
					PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Left);			
				}
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto socketLoc = huchuTongL->tongRight->GetSocketLocation(FName("LimeSocket"));
				auto socketRot = huchuTongL->tongRight-> GetSocketRotation(FName("LimeSocket"));
				GetWorld()->SpawnActor<ASlicedOrange>(slicedOrangeFac, socketLoc, socketRot, param);
			}
			else if(halfSlicedOrangeVat)
			{
				// Haptic Feedback
				if(PC)
				{
					PC->PlayHapticEffect(HF_GrabObjectRight, EControllerHand::Left);			
				}
				FActorSpawnParameters param;
				param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				auto socketLoc = huchuTongL->tongRight->GetSocketLocation(FName("LimeSocket"));
				auto socketRot = huchuTongL->tongRight-> GetSocketRotation(FName("LimeSocket"));
				GetWorld()->SpawnActor<AHalfSlicedOrange>(halfSlicedOrangeFac, socketLoc, socketRot, param);
			}
			isVatCastedLeft=false;
		}
		// 중심점
		FVector Center = huchuTongL->tongRight->GetSocketLocation(FName("TongAttach"));
		// 집게에 집는 대상 오브젝트가 오버랩되었는지 판단하는 OverlapMulti
		// 충돌한 물체를 기억할 배열
		TArray<FOverlapResult> HitObj;
		bool bHit = GetWorld()->OverlapMultiByChannel(HitObj, Center, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(TongsGrabRange), params);
		if (bHit == false)
		{
			return;
		}
		// 가장 가까운 물체를 잡도록 하자
		// 가장 가까운 물체 인덱스
		int32 Closest = 0;
		for (int i = 0; i < HitObj.Num(); ++i)
		{
			// 1. 물리기능이 활성화 되어 있는 대상만 판단
			if (HitObj[i].GetComponent()->IsSimulatingPhysics() == false)
			{
				continue;
			}
			// 잡기에 성공했다
			isGrabbingWithTongsLeft = true;
			// 2.. 현재 손과 가장 가까운 대상과 이번에 검출할 대상과 더 가까운 대상이 있다면		
			// 필요속성 : 현재 가장 가까운 대상과 손과의 거리
			float ClosestDist = FVector::Dist(HitObj[Closest].GetActor()->GetActorLocation(), Center);
			// 필요속성 : 이번에 검출할 대상과 손과의 거리
			float NextDist = FVector::Dist(HitObj[i].GetActor()->GetActorLocation(), Center);

			// 3. 만약 이번 대상이 현재 대상보다 가깝다면,
			if (NextDist < ClosestDist)
			{

				// 가장 가까운 대상으로 변경하기
				Closest = i;
			}
		}
		if (isGrabbingWithTongsLeft&&HitObj[Closest].GetComponent()->IsSimulatingPhysics() == true)
		{
			GrabbedActorWithTongsLeft=HitObj[Closest].GetComponent()->GetAttachmentRootActor();
			GrabbedObjectWithTongsLeft = HitObj[Closest].GetComponent();
			iceCubeL=Cast<AIceCube>(GrabbedActorWithTongsLeft);
			slicedLimeL=Cast<ASlicedLime>(GrabbedActorWithTongsLeft);
			halfSlicedLimeL=Cast<AHalfSlicedLime>(GrabbedActorWithTongsLeft);
			olivePickL=Cast<AOlivePick>(GrabbedActorWithTongsLeft);
			slicedOrangeL=Cast<ASlicedOrange>(GrabbedActorWithTongsLeft);
			halfSlicedOrangeL=Cast<AHalfSlicedOrange>(GrabbedActorWithTongsLeft);
			if(GrabbedActorWithTongsLeft==iceCubeL&&iceCubeL!=nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), TongGrabSound, 1, 1, 0);
				isGrabbingIceWithTongsLeft=true;
				GrabbedObjectWithTongsLeft->SetSimulatePhysics(false);
				GrabbedObjectWithTongsLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				GrabbedObjectWithTongsLeft->AttachToComponent(huchuTongL->tongRight,FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("TongGrabSizeSocket"));
			}
			else if(GrabbedActorWithTongsLeft==slicedLimeL&&slicedLimeL!=nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), TongGrabSound, 1, 1, 0);
				isGrabbingLimeWithTongsLeft=true;
				GrabbedObjectWithTongsLeft->SetSimulatePhysics(false);
				GrabbedObjectWithTongsLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				GrabbedObjectWithTongsLeft->AttachToComponent(huchuTongL->tongRight,FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LimeSocket"));
			}
			else if(GrabbedActorWithTongsLeft==halfSlicedLimeL&&halfSlicedLimeL!=nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), TongGrabSound, 1, 1, 0);
				isGrabbingLimeWithTongsLeft=true;
				GrabbedObjectWithTongsLeft->SetSimulatePhysics(false);
				GrabbedObjectWithTongsLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				GrabbedObjectWithTongsLeft->AttachToComponent(huchuTongL->tongRight,FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LimeSocket"));
			}
			else if(GrabbedActorWithTongsLeft==olivePickL&&olivePickL!=nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), TongGrabSound, 1, 1, 0);
				isGrabbingOliveWithTongsLeft=true;
				GrabbedObjectWithTongsLeft->SetSimulatePhysics(false);
				GrabbedObjectWithTongsLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				GrabbedObjectWithTongsLeft->AttachToComponent(huchuTongL->tongRight,FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("OliveSocket"));
			}
			else if(GrabbedActorWithTongsLeft==slicedOrangeL&&slicedOrangeL!=nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), TongGrabSound, 1, 1, 0);
				isGrabbingOrangeWithTongsLeft=true;
				GrabbedObjectWithTongsLeft->SetSimulatePhysics(false);
				GrabbedObjectWithTongsLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				GrabbedObjectWithTongsLeft->AttachToComponent(huchuTongL->tongRight,FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LimeSocket"));
			}
			else if(GrabbedActorWithTongsLeft==halfSlicedOrangeL&&halfSlicedOrangeL!=nullptr)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), TongGrabSound, 1, 1, 0);
				isGrabbingOrangeWithTongsLeft=true;
				GrabbedObjectWithTongsLeft->SetSimulatePhysics(false);
				GrabbedObjectWithTongsLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				GrabbedObjectWithTongsLeft->AttachToComponent(huchuTongL->tongRight,FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LimeSocket"));
			}
			
		}	

			auto tongCompRef = huchuTongL->tongRight;
			auto tongLoc =  huchuTongL->tongRight->GetSocketLocation(FName("TongGrabSizeSocket"));
			auto tongRightVector = huchuTongL->GetActorForwardVector();
			FCollisionQueryParams params1;
			params1.AddIgnoredActor(huchuTongL);  
			params1.AddIgnoredActor(this);				
			FHitResult leftTrace;
			FHitResult rightTrace;
			// Tongs 양쪽에서 LineTrace롤 통해 Grab할 대상의 크기를 측정한다.
			bool bHitR = GetWorld()->LineTraceSingleByChannel(rightTrace,tongLoc+tongRightVector*25.0f, tongLoc+tongRightVector*-25.0f, ECC_Visibility,params1);
			bool bHitL = GetWorld()->LineTraceSingleByChannel(leftTrace,tongLoc+tongRightVector*-25.0f, tongLoc+tongRightVector*25.0f, ECC_Visibility,params1);
			// LineTrace가 양쪽 모두 적중했다면
			if(bHitL&&bHitR)
			{
				if(isGrabbingIceWithTongsLeft)
				{
					isTongsTickEnabledL = false;
					tongCompRef->SetRelativeRotation(FRotator(5, 0, 0));
				}
				else if(isGrabbingLimeWithTongsLeft)
				{
					isTongsTickEnabledL = false;
					tongCompRef->SetRelativeRotation(FRotator(13, 0, 0));
				}
				else if(isGrabbingOliveWithTongsLeft)
				{
					isTongsTickEnabledL = false;
					tongCompRef->SetRelativeRotation(FRotator(12, 0, 0));
				}
				else if(isGrabbingOrangeWithTongsLeft)
				{
					isTongsTickEnabledL = false;
					tongCompRef->SetRelativeRotation(FRotator(13, 0, 0));
				}
			}
			// LineTrace가 적중하지 않았다면 -> 허공이라면
			else
			{
				// Oculus Trigger Input Value에 따른 Tongs Rotation 제어 Tick 활성화
				isTongsTickEnabledL=true;
			}

	}
	if(isGrabbingCupLeft&&cupL!=nullptr)
	{
		cupL->SetCupEmpty();
	}
}

void APlayerCharacter::FireReleasedRight()
{
	//클릭 버튼 떼어내기 위한 함수
	if (widgetInteractionComp)
	{
		widgetInteractionComp->ReleasePointerKey(FKey(FName("LeftMouseButton")));
		bool isCursorOn = widgetInteractionComp->IsOverHitTestVisibleWidget();
		if(isCursorOn)
		{
			UGameplayStatics::PlaySound2D(GetWorld(),TriggerReleasedSound, 1, 1, 0);
		}
	}
	if(isGrabbingTongsRight)
	{		
		// Tongs로 잡고 있는 대상이 있었다면
		if (isGrabbingWithTongsRight)
		{
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;	
			auto tongCompRef = huchuTong->tongRight;
			if(isGrabbingIceWithTongsRight)
			{
				tongCompRef->SetRelativeRotation(FRotator(0, 0, 0));
				//UKismetSystemLibrary::MoveComponentTo(tongCompRef, tongCompRef->GetRelativeLocation(), tongCompRef->GetRelativeRotation()+FRotator(-5, 0, 0), false, false, 0.0, false, EMoveComponentAction::Move, LatentInfo);
				if(iceCube!=nullptr)
				{
					iceCube->isIceCubeAttachable=true;
				}
			}
			else if(isGrabbingLimeWithTongsRight)
			{
				tongCompRef->SetRelativeRotation(FRotator(0, 0, 0));
				//UKismetSystemLibrary::MoveComponentTo(tongCompRef, tongCompRef->GetRelativeLocation(), tongCompRef->GetRelativeRotation()+FRotator(-13, 0, 0), false, false, 0.0, false, EMoveComponentAction::Move, LatentInfo);
				if(halfSlicedLime!=nullptr)
				{
					halfSlicedLime->isHalfSlicedLimeAttachable=true;
				}
				if(slicedLime!=nullptr)
				{
					slicedLime->isSlicedLimeAttachable=true;
				}
			}
			else if(isGrabbingOliveWithTongsRight)
			{
				tongCompRef->SetRelativeRotation(FRotator(0, 0, 0));
				//UKismetSystemLibrary::MoveComponentTo(tongCompRef, tongCompRef->GetRelativeLocation(), tongCompRef->GetRelativeRotation()+FRotator(-12, 0, 0), false, false, 0.0, false, EMoveComponentAction::Move, LatentInfo);
				if(olivePick!=nullptr)
				{
					olivePick->isOliveAttachable = true;
				}
			}
			else if(isGrabbingOrangeWithTongsRight)
			{
				tongCompRef->SetRelativeRotation(FRotator(0, 0, 0));
				//UKismetSystemLibrary::MoveComponentTo(tongCompRef, tongCompRef->GetRelativeLocation(), tongCompRef->GetRelativeRotation()+FRotator(-13, 0, 0), false, false, 0.0, false, EMoveComponentAction::Move, LatentInfo);
				if(halfSlicedOrange!=nullptr)
				{
					halfSlicedOrange->isHalfSlicedOrangeAttachable=true;
				}
				if(slicedOrange!=nullptr)
				{
					slicedOrange->isSlicedOrangeAttachable=true;
				}
			}
			isTongsTickEnabled = true;
			// 1. 잡지않은 상태로 전환
			isGrabbingWithTongsRight = false;
			isGrabbingIceWithTongsRight=false;
			isGrabbingLimeWithTongsRight=false;
			isGrabbingOliveWithTongsRight=false;
			if(GrabbedObjectWithTongsRight!=nullptr)
			{
				// 2. 손에서 떼어내기
				GrabbedObjectWithTongsRight->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				// 3. 물리기능 활성화
				GrabbedObjectWithTongsRight->SetSimulatePhysics(true);
				// 4. 충돌기능 활성화
				GrabbedObjectWithTongsRight->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				GrabbedObjectWithTongsRight = nullptr;
			}
		}
		// Tongs 로 잡고 있는 대상이 없었다면
		else
		{
			isGrabbingWithTongsRight = false;
			isGrabbingIceWithTongsRight=false;
			isGrabbingLimeWithTongsRight=false;
			isGrabbingOliveWithTongsRight=false;
			isGrabbingOrangeWithTongsRight=false;
			isTongsTickEnabled = true;
		}	
	}
	else
	{
		return;
	}
}

void APlayerCharacter::FireReleasedLeft()
{
	if (widgetInteractionCompLeft)
	{
		widgetInteractionCompLeft->ReleasePointerKey(FKey(FName("LeftMouseButton")));
		bool isCursorOn = widgetInteractionComp->IsOverHitTestVisibleWidget();
		if(isCursorOn)
		{
			UGameplayStatics::PlaySound2D(GetWorld(),TriggerReleasedSound, 1, 1, 0);
		}
	}
	if(isGrabbingTongsLeft)
	{		
		// Tongs로 잡고 있는 대상이 있었다면
		if (isGrabbingWithTongsLeft)
		{
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;
			auto tongCompRef = huchuTongL->tongRight;
			if(isGrabbingIceWithTongsLeft)
			{
				tongCompRef->SetRelativeRotation(FRotator(0, 0, 0));
				//UKismetSystemLibrary::MoveComponentTo(tongCompRef, tongCompRef->GetRelativeLocation(), tongCompRef->GetRelativeRotation()+FRotator(-5, 0, 0), false, false, 0.0, false, EMoveComponentAction::Move, LatentInfo);
				if(iceCubeL!=nullptr)
				{
					iceCubeL->isIceCubeAttachable=true;
				}
			}
			else if(isGrabbingLimeWithTongsLeft)
			{
				tongCompRef->SetRelativeRotation(FRotator(0, 0, 0));
				//UKismetSystemLibrary::MoveComponentTo(tongCompRef, tongCompRef->GetRelativeLocation(), tongCompRef->GetRelativeRotation()+FRotator(-13, 0, 0), false, false, 0.0, false, EMoveComponentAction::Move, LatentInfo);
				if(halfSlicedLimeL!=nullptr)
				{
					halfSlicedLimeL->isHalfSlicedLimeAttachable=true;
				}
				if(slicedLimeL!=nullptr)
				{
					slicedLimeL->isSlicedLimeAttachable=true;
				}
			}
			else if(isGrabbingOliveWithTongsLeft)
			{
				tongCompRef->SetRelativeRotation(FRotator(0, 0, 0));
				//UKismetSystemLibrary::MoveComponentTo(tongCompRef, tongCompRef->GetRelativeLocation(), tongCompRef->GetRelativeRotation()+FRotator(-12, 0, 0), false, false, 0.0, false, EMoveComponentAction::Move, LatentInfo);
				if(olivePickL!=nullptr)
				{
					olivePickL->isOliveAttachable=true;
				}
			}
			else if(isGrabbingOrangeWithTongsLeft)
			{
				tongCompRef->SetRelativeRotation(FRotator(0, 0, 0));
				//UKismetSystemLibrary::MoveComponentTo(tongCompRef, tongCompRef->GetRelativeLocation(), tongCompRef->GetRelativeRotation()+FRotator(-13, 0, 0), false, false, 0.0, false, EMoveComponentAction::Move, LatentInfo);
				if(halfSlicedOrangeL!=nullptr)
				{
					halfSlicedOrangeL->isHalfSlicedOrangeAttachable=true;
				}
				if(slicedOrangeL!=nullptr)
				{
					slicedOrangeL->isSlicedOrangeAttachable=true;
				}
			}
			isTongsTickEnabledL = true;
			// 1. 잡지않은 상태로 전환
			isGrabbingWithTongsLeft = false;
			isGrabbingIceWithTongsLeft=false;
			isGrabbingLimeWithTongsLeft=false;
			isGrabbingOliveWithTongsLeft=false;
			if(GrabbedObjectWithTongsLeft!=nullptr)
			{
				// 2. 손에서 떼어내기
				GrabbedObjectWithTongsLeft->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				// 3. 물리기능 활성화
				GrabbedObjectWithTongsLeft->SetSimulatePhysics(true);
				// 4. 충돌기능 활성화
				GrabbedObjectWithTongsLeft->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				GrabbedObjectWithTongsLeft = nullptr;
			}
		}
		// Tongs 로 잡고 있는 대상이 없었다면
		else
		{
			isGrabbingWithTongsLeft = false;
			isGrabbingIceWithTongsLeft=false;
			isGrabbingLimeWithTongsLeft=false;
			isGrabbingOliveWithTongsLeft=false;
			isGrabbingOrangeWithTongsLeft=false;
			isTongsTickEnabledL = true;
		}	
	}
	else
	{
		return;
	}
}

void APlayerCharacter::ShowMenuWidget()
{
	if(BarGameInstance->bCheckMenu!=false)
	{
		if(menuWidgetBool==false)
		{
			gameMode->SpawnMenu();
			UGameplayStatics::PlaySound2D(GetWorld(), MenuWidgetAppearSound, 1, 1, 0);
			menuWidgetBool=true;
		}
		else
		{
			gameMode->DestroyMenu();
			UGameplayStatics::PlaySound2D(GetWorld(), MenuWidgetDisappearSound, 1, 1, 0);
			menuWidgetBool=false;
		}
	}

}

void APlayerCharacter::ShowGuideWidget()
{
	UGameplayStatics::PlaySound2D(GetWorld(), GuideWidgetAppearSound, 1, 1, 0);

	FVector rot1 = VRReplicatedCamera->GetComponentLocation();
	FVector rot2 = VRReplicatedCamera->GetComponentLocation() + VRReplicatedCamera->GetForwardVector() * 80;
	FRotator rotend = UKismetMathLibrary::FindLookAtRotation(rot1, rot2);
	FRotator rot = {0, rotend.Yaw - 180, 0};
	FVector loc1 = VRReplicatedCamera->GetComponentLocation() + VRReplicatedCamera->GetForwardVector() * 150;
	FVector loc2 = GetActorLocation() + GetActorUpVector() * 130;
	FVector loc = {loc1.X, loc1.Y, loc2.Z};
	if(BarGameInstance)
	{
		if(BarGameInstance->bEnglish==false)
		{
			GuideWidget = GetWorld()->SpawnActor<AGuideWidgetActor>(guideFactory, loc, rot);
		}
		else
		{
			GuideWidget = GetWorld()->SpawnActor<AGuideWidgetActor>(guideFactoryEng, loc, rot);
		}
	}

}

void APlayerCharacter::HideGuideWidget()
{
	if(GuideWidget!=nullptr)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), GuideWidgetDisappearSound, 1, 1, 0);

		GuideWidget->Destroy();
	}
}

void APlayerCharacter::TestInputKey()
{
	gameMode->SpawnMenu();
}

void APlayerCharacter::PlayerTutoText()
{
	// 플레이어 대사 1
	playerText_UI->SetSwitcher(0);
	playerText_UI->StartPlayerText(1);
	playerText_UI->StartPlayer();

	PlayPlayerSound(0);

	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &APlayerCharacter::EndText, 3.0f, false);
}

void APlayerCharacter::PlayerStartText()
{
	if(BarGameInstance->checkDayCount == 1 && UGameplayStatics::GetCurrentLevelName(GetWorld()) == "BarStartMap")
	{
		// 플레이어 대사 2
		playerText_UI->SetSwitcher(0);
		playerText_UI->StartPlayerText(2);
		playerText_UI->StartPlayer();

		PlayPlayerSound(1);
	}
	else if(BarGameInstance->checkDayCount == 2 && UGameplayStatics::GetCurrentLevelName(GetWorld()) == "BarStartMap")
	{
		// 플레이어 대사 3
		playerText_UI->SetSwitcher(0);
		playerText_UI->StartPlayerText(3);
		playerText_UI->StartPlayer();
		
		PlayPlayerSound(2);
	}

	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &APlayerCharacter::EndText, 3.0f, false);
}

void APlayerCharacter::PlayerMenuText()
{
	// 플레이어 대사 0
	playerText_UI->SetSwitcher(0);
	playerText_UI->StartPlayerText(0);
	playerText_UI->StartPlayer();

	FTimerHandle timer;
	GetWorldTimerManager().SetTimer(timer, this, &APlayerCharacter::EndText, 3.0f, false);
}

void APlayerCharacter::EndText()
{
	playerText_UI->EndPlayer();
}

void APlayerCharacter::PlayPlayerSound(int32 idx)
{
	UGameplayStatics::PlaySound2D(GetWorld(), playerSound[idx]);
}
