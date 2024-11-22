// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/OptionalRepSkeletalMeshActor.h"
#include "GraspingHandRealistic.generated.h"

/**
 * 
 */
UCLASS()
class BARSIM_API AGraspingHandRealistic : public AOptionalRepGrippableSkeletalMeshActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AGraspingHandRealistic(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerSettings")
	class UWidgetComponent* actorInfoWidgetComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerSettings")
	class UActorInfoWidget* actorInfoWidget;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	class USkeletalMeshComponent* skeletalHand;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent*OverlappedComponent, AActor*OtherActor, UPrimitiveComponent*OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void RespawnGraspingHands();	

	// Game Instance
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBarGameInstance* BarGameInstance;
	
	// References
	UPROPERTY()
	class AHuchuTong* huchuTong;
	UPROPERTY()
	class ABottleBase* bottle;
	UPROPERTY()
	class ATablet* tablet;
	UPROPERTY()
	class ACoaster* coaster;
	UPROPERTY()
	class ACupBase* cup;
	UPROPERTY()
	class AIceCube* iceCube;
	UPROPERTY()
	class ABarSpoon* barSpoon;
	UPROPERTY()
	class AShakerLid* shakerLid;
	UPROPERTY()
	class AStrainer* strainer;
	UPROPERTY()
	class AShakerStrainer* shakerStrainer;
	UPROPERTY()
	class AShaker* shaker;
	UPROPERTY()
	class AMixingGlass* mixingGlass;
	UPROPERTY()
	class ASlicedLime* slicedLime;
	UPROPERTY()
	class AHalfSlicedLime* halfSlicedLime;
	UPROPERTY()
	class AHalfSlicedLimeVat* halfSlicedLimeVat;
	UPROPERTY()
	class ASlicedLimeVat* slicedLimeVat;
	UPROPERTY()
	class AIceCubeVat* iceCubeVat;
	UPROPERTY()
	class AMartiniCup* martiniCup;
	UPROPERTY()
	class ARockGlass* rockGlass;
	UPROPERTY()
	class AHalfSlicedOrangeVat* halfSlicedOrangeVat;
	UPROPERTY()
	class AHalfSlicedOrange* halfSlicedOrange;
	UPROPERTY()
	class ASlicedOrangeVat* SlicedOrangeVat;
	UPROPERTY()
	class ASlicedOrange* SlicedOrange;
	UPROPERTY()
	class ADrinkCan* DrinkCan;

	// Player Controller
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class APlayerController* PC;
	
	// Haptic Feedback
	UPROPERTY(EditDefaultsOnly, Category="Haptics")
	class UHapticFeedbackEffect_Curve* HF_ActorOverlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle widgetDestroyHandle;
};
