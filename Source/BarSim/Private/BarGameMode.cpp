// Fill out your copyright notice in the Description page of Project Settings.


#include "BarGameMode.h"
#include "MenuWidgetActor.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

ABarGameMode::ABarGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ABarGameMode::BeginPlay()
{
	Super::BeginPlay();

}

void ABarGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void ABarGameMode::SpawnMenu()
{
	auto player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	
	if(player != nullptr)
	{
		FRotator rot = player->GetActorRotation()+FRotator(0, 180, 0);
		FVector loc = player->GetActorLocation()+(player->GetActorForwardVector()*100);
		GetWorld()->SpawnActor<AMenuWidgetActor>(menuFactory, loc, rot);
	}
}
