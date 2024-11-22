// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class BARSIM_API APlayerCharacter : public AVRCharacter
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Widget Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
	class UWidgetInteractionComponent* widgetInteractionComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
	class UWidgetInteractionComponent* widgetInteractionCompLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerSettings")
	class UWidgetComponent* playerTextWidget;
	UPROPERTY()
	class UPlayerDialogWidget* playerText_UI;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UNiagaraComponent* WidgetTraceRight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UNiagaraComponent* WidgetTraceLeft;

	
	// Aims
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HandComp")
	class UMotionControllerComponent* RightAim;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HandComp")
	class UMotionControllerComponent* LeftAim;

	// Inputs
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* VREInputMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Hand;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* UseHeldObjectRight;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* UseHeldObjectLeft;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ShowMenu;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ShowGuide;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* TestInput;
	
	// Functions
	UFUNCTION(BlueprintCallable)
	void CheckGrabbedObjectRight();
	UFUNCTION(BlueprintCallable)
	void CheckGrabbedObjectLeft();
	UFUNCTION(BlueprintCallable)
	void CheckDroppedObjectRight();
	UFUNCTION(BlueprintCallable)
	void CheckDroppedObjectLeft();
	UFUNCTION(BlueprintCallable)
	void DestroyDroppedObjectRight();
	UFUNCTION(BlueprintCallable)
	void DestroyDroppedObjectLeft();
	void FireRight();
	void FireLeft();
	void FireReleasedRight();
	void FireReleasedLeft();
	void ShowMenuWidget();
	void ShowGuideWidget();
	void HideGuideWidget();
	void TestInputKey();
	UFUNCTION(BlueprintImplementableEvent)
	void ResetGraspingHands();

	// Grabbing Boolean
	bool IsGrabbedLeft = false;
	bool IsGrabbedRight = false;
	bool isGrabbingTongsLeft = false;
	bool isGrabbingTongsRight = false;
	bool isGrabbingWithTongsRight=false;
	bool isGrabbingWithTongsLeft=false;
	bool isTongsTickEnabled = true;
	bool isTongsTickEnabledL = true;
	bool isRecipeMode = false;
	bool isGrabbingBottleRight = false;
	bool isGrabbingBottleLeft = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isGrabbingTabletRight = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isGrabbingTabletLeft = false;
	bool isGrabbingCoasterRight=false;
	bool isGrabbingCoasterLeft = false;
	bool isGrabbingCupLeft = false;
	bool isGrabbingCupRight = false;
	bool isGrabbingBarSpoonLeft=false;
	bool isGrabbingBarSpoonRight=false;
	bool isGrabbingShakerRight=false;
	bool isGrabbingShakerLeft=false;
	bool isGrabbingShakerStrainerRight=false;
	bool isGrabbingShakerStrainerLeft=false;
	bool isGrabbingStrainerRight=false;
	bool isGrabbingStrainerLeft=false;
	bool isGrabbingMixingGlassRight=false;
	bool isGrabbingMixingGlassLeft=false;
	bool isGrabbingShakerLidRight=false;
	bool isGrabbingShakerLidLeft=false;
	bool isGrabbingIceWithTongsRight=false;
	bool isGrabbingIceWithTongsLeft=false;
	bool isGrabbingLimeWithTongsRight=false;
	bool isGrabbingLimeWithTongsLeft=false;
	bool isGrabbingOliveWithTongsRight=false;
	bool isGrabbingOliveWithTongsLeft=false;
	bool isGrabbingDrinkCanRight = false;
	bool isGrabbingDrinkCanLeft = false;
	bool isGrabbingOrangeWithTongsRight = false;
	bool isGrabbingOrangeWithTongsLeft = false;
	bool isGrabbingSlicedLimeVatRight = false;
	bool isGrabbingSlicedLimeVatLeft = false;
	bool isGrabbingHalfSlicedLimeVatRight = false;
	bool isGrabbingHalfSlicedLimeVatLeft = false;
	bool isGrabbingSlicedOrangeVatRight = false;
	bool isGrabbingSlicedOrangeVatLeft = false;
	bool isGrabbingHalfSlicedOrangeVatRight =false;
	bool isGrabbingHalfSlicedOrangeVatLeft =false;
	bool isGrabbingOliveVatRight = false;
	bool isGrabbingOliveVatLeft = false;
	bool isGrabbingIceCubeVatRight =false;
	bool isGrabbingIceCubeVatLeft =false;
	
	
	bool isVatCastedRight = false;
	bool isVatCastedLeft = false;
	

	bool menuWidgetBool = false;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	double fingerPressedActionValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	double fingerPressedActionValueLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	double grabPressedActionValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	double grabPressedActionValueLeft;


	UPROPERTY(EditDefaultsOnly)
	float TongsGrabRange = 10;
	
	// 오른손 그랩 액터
	UPROPERTY()
	class AActor* GrabbedActorRight;
	// 왼손 그랩 액터
	UPROPERTY()
	class AActor* GrabbedActorLeft;

	// Tongs를 통한 그랩 오른쪽 컴포넌트
	UPROPERTY()
	class UPrimitiveComponent* GrabbedObjectWithTongsRight;
	UPROPERTY()
	class AActor* GrabbedActorWithTongsRight;
	// Tongs를 통한 그랩 왼쪽 컴포넌트
	UPROPERTY()
	class UPrimitiveComponent* GrabbedObjectWithTongsLeft;
	UPROPERTY()
	class AActor* GrabbedActorWithTongsLeft;
	// Player Controller
	UPROPERTY()
	class APlayerController* PC;
	// Game Instance
	UPROPERTY()
	class UBarGameInstance* BarGameInstance;

	// References
	UPROPERTY()
	class AHuchuTong* huchuTong;
	UPROPERTY()
	class AHuchuTong* huchuTongL;
	UPROPERTY()
	class ABottleBase* bottle;
	UPROPERTY()
	class ABottleBase* bottleL;
	UPROPERTY()
	class ATablet* tablet;
	UPROPERTY()
	class ATablet* tabletL;
	UPROPERTY()
	class ACoaster* coaster;
	UPROPERTY()
	class ACoaster* coasterL;
	UPROPERTY()
	class ACupBase* cup;
	UPROPERTY()
	class ACupBase* cupL;
	UPROPERTY()
	class AIceCube* iceCube;
	UPROPERTY()
	class AIceCube* iceCubeL;
	UPROPERTY()
	class ABarSpoon* barSpoon;
	UPROPERTY()
	class ABarSpoon* barSpoonL;
	UPROPERTY()
	class AShakerLid* shakerLid;
	UPROPERTY()
	class AShakerLid* shakerLidL;
	UPROPERTY()
	class AStrainer* strainer;
	UPROPERTY()
	class AStrainer* strainerL;
	UPROPERTY()
	class AShakerStrainer* shakerStrainer;
	UPROPERTY()
	class AShakerStrainer* shakerStrainerL;
	UPROPERTY()
	class AShaker* shaker;
	UPROPERTY()
	class AShaker* shakerL;
	UPROPERTY()
	class AMixingGlass* mixingGlass;
	UPROPERTY()
	class AMixingGlass* mixingGlassL;
	UPROPERTY()
	class ASlicedLime* slicedLime;
	UPROPERTY()
	class ASlicedLime* slicedLimeL;
	UPROPERTY()
	class AHalfSlicedLime* halfSlicedLime;
	UPROPERTY()
	class AHalfSlicedLime* halfSlicedLimeL;
	UPROPERTY()
	class ABarGameMode* gameMode;
	UPROPERTY()
	class AHalfSlicedLimeVat* halfSlicedLimeVat;
	UPROPERTY()
	class ASlicedLimeVat* slicedLimeVat;
	UPROPERTY()
	class AIceCubeVat* iceCubeVat;
	UPROPERTY()
	class AOlivePick* olivePick;
	UPROPERTY()
	class AOlivePick* olivePickL;
	UPROPERTY()
	class AOliveVat* oliveVat;
	UPROPERTY()
	class ADrinkCan* drinkCan;
	UPROPERTY()
	class ADrinkCan* drinkCanL;
	UPROPERTY()
	class ASlicedOrange* slicedOrange;
	UPROPERTY()
	class ASlicedOrange* slicedOrangeL;
	UPROPERTY()
	class AHalfSlicedOrange* halfSlicedOrange;
	UPROPERTY()
	class AHalfSlicedOrange* halfSlicedOrangeL;
	UPROPERTY()
	class AHalfSlicedOrangeVat* halfSlicedOrangeVat;
	UPROPERTY()
	class ASlicedOrangeVat* slicedOrangeVat;
	UPROPERTY()
	class AHalfSlicedLimeVat* halfSlicedLimeVatL;
	UPROPERTY()
	class ASlicedLimeVat* slicedLimeVatL;
	UPROPERTY()
	class AIceCubeVat* iceCubeVatL;
	UPROPERTY()
	class AHalfSlicedOrangeVat* halfSlicedOrangeVatL;
	UPROPERTY()
	class ASlicedOrangeVat* slicedOrangeVatL;
	UPROPERTY()
	class AOliveVat* oliveVatL;
	
	// Sounds
	UPROPERTY(EditAnywhere, Category="PlayerSounds")
	class USoundBase* grabSound;

	UPROPERTY(EditAnywhere, Category="PlayerSounds")
	class USoundBase* TongGrabSound;

	UPROPERTY(EditAnywhere, Category="PlayerSounds")
	class USoundBase* TriggerPressedSound;

	UPROPERTY(EditAnywhere, Category="PlayerSounds")
	class USoundBase* TriggerReleasedSound;
	
	UPROPERTY(EditAnywhere, Category="PlayerSounds")
	class USoundBase* MenuWidgetAppearSound;

	UPROPERTY(EditAnywhere, Category="PlayerSounds")
	class USoundBase* MenuWidgetDisappearSound;

	UPROPERTY(EditAnywhere, Category="PlayerSounds")
	class USoundBase* GuideWidgetAppearSound;

	UPROPERTY(EditAnywhere, Category="PlayerSounds")
	class USoundBase* GuideWidgetDisappearSound;
	

	// Lime BP
	UPROPERTY(EditAnywhere, Category="Factory")
	TSubclassOf<class AHalfSlicedLime> halfSlicedLimeFac;

	UPROPERTY(EditAnywhere, Category="Factory")
	TSubclassOf<class ASlicedLime> slicedLimeFac;

	// Ice Cube BP
	UPROPERTY(EditAnywhere, Category="Factory")
	TSubclassOf<class AIceCube> iceCubeFac;

	// Olive BP
	UPROPERTY(EditAnywhere, Category="Factory")
	TSubclassOf<class AOlivePick> oliveFac;

	// Orange BP
	UPROPERTY(EditAnywhere, Category="Factory")
	TSubclassOf<class AHalfSlicedOrange> halfSlicedOrangeFac;

	UPROPERTY(EditAnywhere, Category="Factory")
	TSubclassOf<class ASlicedOrange> slicedOrangeFac;

	// Haptic Feedback
	UPROPERTY(EditDefaultsOnly, Category="Haptics")
	class UHapticFeedbackEffect_Curve* HF_GrabObjectRight;

	UPROPERTY(EditDefaultsOnly, Category="Haptics")
	class UHapticFeedbackEffect_Curve* HF_GrabObjectLeft;

	
	// Camera Fade
	UFUNCTION(BlueprintImplementableEvent)
	void StartCameraFade();

	
	// 대사
	UFUNCTION(BlueprintCallable)
	void PlayerTutoText();
	
	UFUNCTION()
	void PlayerStartText();

	UFUNCTION()
	void PlayerMenuText();

	UPROPERTY()
	bool isDropped = false;

	UFUNCTION()
	void EndText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=sound)
	TArray<USoundBase*> playerSound;

	UFUNCTION()
	void PlayPlayerSound(int32 idx);

	UPROPERTY()
	class AGuideWidgetActor* GuideWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGuideWidgetActor> guideFactory;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AGuideWidgetActor> guideFactoryEng;
};
