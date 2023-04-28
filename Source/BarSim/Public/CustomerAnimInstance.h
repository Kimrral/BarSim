// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CustomerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BARSIM_API UCustomerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	class ACustomerCharacter* owner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float forwardVelocity;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* customerMontageFactory;

	UFUNCTION()
	void OnSitAnim(FName sectionName);

	UFUNCTION(BlueprintCallable)
	void EndTalking();

	UFUNCTION(BlueprintCallable)
	void EndWaitLong();

	UFUNCTION(BlueprintCallable)
	void EndDrinking();

	UFUNCTION(BlueprintCallable)
	void EndBad();

	UFUNCTION(BlueprintCallable)
	void EndGood();
};
