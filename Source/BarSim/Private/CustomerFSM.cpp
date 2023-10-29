// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomerFSM.h"
#include "AIController.h"
#include "BarGameInstance.h"
#include "CupBase.h"
#include "CustomerAnimInstance.h"
#include "CustomerCharacter.h"
#include "CustomerOrderWidget.h"
#include "HalfSlicedLime.h"
#include "HalfSlicedOrange.h"
#include "OlivePick.h"
#include "PlayerCharacter.h"
#include "SlicedOrange.h"
#include "SpawnManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCustomerFSM::UCustomerFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCustomerFSM::BeginPlay()
{
	Super::BeginPlay();

	owner = Cast<ACustomerCharacter>(GetOwner());

	ai = Cast<AAIController>(owner->GetController());

	spawnManager = Cast<ASpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnManager::StaticClass()));

	player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	
	// 기본 상태 설정
	state = ECustomerState::IDLE;

	sitState = ECustomerSitState::STANDBY;

	drinkState = ECustomerDrinkState::IDLE;

	// 걷기 속도 조절
	owner->GetCharacterMovement()->MaxWalkSpeed = 200.0f;

	// 주문 칵테일 정하기
	SetOrderCoctail();
}


// Called every frame
void UCustomerFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (state)
	{
	case ECustomerState::IDLE:
		TickIdle();
		break;
	case ECustomerState::MOVE:
		TickMove();
		break;
	case ECustomerState::READYSIT:
		TickReadySit();
		break;
	case ECustomerState::SIT:
		TickSit();
		break;
	case ECustomerState::LEAVE:
		TickLeave();
		break;
	}
}

// 손님 의자에 attach
void UCustomerFSM::AttachCustomer()
{
	owner->GetCapsuleComponent()->SetEnableGravity(false);

	owner->AttachToComponent(spawnManager->aChairs[idx]->sitComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

// 손님 의자에 detach
void UCustomerFSM::DetachCustomer()
{
	owner->GetCapsuleComponent()->SetEnableGravity(true);

	owner->DetachAllSceneComponents(spawnManager->aChairs[idx]->sitComp, FDetachmentTransformRules::KeepWorldTransform);
}

// 주문 칵테일 정하기
void UCustomerFSM::SetOrderCoctail()
{
	if(owner != nullptr)
	{
		// 랜덤으로 메뉴 선정
		int32 result = FMath::RandRange(1,8);
			
		if(result > 6)
		{
			// 진라임
			orderIdx = 1;
		}
		else if(result <= 6 && result > 4)
		{
			// 다이키리
			orderIdx = 2;
		}
		else if(result <= 4 && result > 2)
		{
			// 마타니
			orderIdx = 3;
		}
		else if(result <= 2)
		{
			// 네그로니
			orderIdx = 4;
		}
	}
}

// ------------------------------------------------------------------------------------idle state
// 상태 설정 함수
void UCustomerFSM::SetState(ECustomerState next)
{
	// 넘어온 상태로 변경
	state = next;

	// 플레이 되는 애니메이션 플레이 체크 초기화
	bCheckPlayAnim = false;

	// 상태를 anim 클래스에 동기화
	owner->customerAnim->ownerState = next;
}

void UCustomerFSM::TickIdle()
{
	if(spawnManager != nullptr)
	{
		for(int i = 0; i<spawnManager->aChairs.Num(); i++)
		{
			// 스폰 매니저에 있는 의자 배열에 착석 여부를 확인하는 배열 체크
			if(spawnManager->aChairs[i]->bCheck == false)
			{
				// 비어 있는 의자가 있으면 의자의 순서를 저장하고
				idx = i;
				
				// 앉은 의자 배열에 착석 여부 바꾸기
				spawnManager->aChairs[idx]->bCheck = true;

				// 가게 오픈했다고 바꾸기
				spawnManager->bCheckSpawn = true;

				// 상태 변경
				SetState(ECustomerState::MOVE);
				
				break;
			}
		}
	}
}

void UCustomerFSM::TickMove()
{
	// 지정 된 의자 뒤로 이동
	auto loc = spawnManager->aChairs[idx]->GetActorLocation() + spawnManager->aChairs[idx]->GetActorForwardVector() * -100;

	auto result = ai->MoveToLocation(loc);

	if(result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		// 도착하면 다음 단계로
		SetState(ECustomerState::READYSIT);
	}
}

void UCustomerFSM::TickReadySit()
{
	// 지정 된 의자 뒤로 이동
	auto loc = spawnManager->aChairs[idx]->GetActorLocation() + spawnManager->aChairs[idx]->GetActorForwardVector() * -20;

	auto result = ai->MoveToLocation(loc);

	if(result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		if(bCheckPlayAnim != true)
		{
			bCheckPlayAnim = true;
		
			// 도착하면 앉는 애니메이션 실행
			owner->customerAnim->OnStandAnim(TEXT("SitStoll"));
		}
	}
}

void UCustomerFSM::TickSit()
{
	// 대기시간 체크를 위한 시간 적립
	curTime += GetWorld()->GetDeltaSeconds();

	switch (sitState)
	{
	case ECustomerSitState::STANDBY:
		TickStandby();
		break;
	case ECustomerSitState::STANDBYWAITLONG:
		TickStandbyWaitLong();
		break;
	case ECustomerSitState::ORDER:
		TickOrder();
		break;
	case ECustomerSitState::WAIT:
		TickWait();
		break;
	case ECustomerSitState::WAITLONG:
		TickWaitLong();
		break;
	case ECustomerSitState::ORDERJUDGE:
		TickOrderJudge();
		break;
	case ECustomerSitState::HOLDCUP:
		TickHoldCup();
		break;
	case ECustomerSitState::DRINK:
		TickDrink();
		break;
	case ECustomerSitState::TASTEJUDGE:
		TickTasteJudge();
		break;
	case ECustomerSitState::ANGRY:
		TickAngry();
		break;
	case ECustomerSitState::AWESOME:
		TickAwesome();
		break;
	case ECustomerSitState::READYLEAVE:
		TickReadyLeave();
		break;
	}
}

void UCustomerFSM::TickLeave()
{
	// 문 밖의 스폰매니저 주변으로 이동
	auto loc = spawnManager->GetActorLocation() + spawnManager->GetActorRightVector() * 600;

	auto result = ai->MoveToLocation(loc);
	
	if(result == EPathFollowingRequestResult::AlreadyAtGoal && spawnManager->bIsCoaster[idx] != true && spawnManager->bIsCoctail[idx] != true)
	{
		// 컵을 치우라는 알림 지우기
		spawnManager->aChairs[idx]->HideScore();
		
		spawnManager->aChairs[idx]->bSameOrder = false;
		
		spawnManager->aChairs[idx]->bUnSameOrder = false;
		
		spawnManager->aChairs[idx]->bOnceOverlap = false;
		
		// 도착하면 의자 배열에 착석 여부 바꾸고 사라짐
		spawnManager->aChairs[idx]->bCheck = false;
		
		owner->Destroy();
	}
}

// -----------------------------------------------------------------------------------------sit
// sit 상태 설정 함수
void UCustomerFSM::SetSitState(ECustomerSitState next)
{
	// 상태 변경
	sitState = next;

	// 상태가 변경 될 때마다 대기시간 초기화
	curTime = 0;

	// 플레이 되는 애니메이션 플레이 체크 초기화
	bCheckPlayAnim = false;

	// 상태를 anim 클래스에 동기화
	owner->customerAnim->ownerSitState = next;

	bCheckCustomer = false;
}

void UCustomerFSM::TickStandby()
{
	if(bCheckCustomer != true && curTime > 1)
	{
		// 오더 위젯에 물음표 띄우기
		owner->order_UI->SetImage(owner->order_UI->iconImage[0]);
		owner->order_UI->SetSwitcher(0);
		owner->order_UI->StartCustomer();

		bCheckCustomer = true;
	}
	
	// 일정 시간 전에 코스터가 있다면 오더로 상태 변경
	if(spawnManager->bIsCoaster[idx] != false)
	{
		owner->order_UI->EndCustomer();
		
		SetSitState(ECustomerSitState::ORDER);
	}
	// 일정 시간 이후에 코스터가 없다면 웨이트롱으로 상태 변경
	/*else if(curTime > 10 && spawnManager->bIsCoaster[idx] != true)
	{
		SetSitState(ECustomerSitState::STANDBYWAITLONG);
	}*/
}

void UCustomerFSM::TickStandbyWaitLong()
{
	// 오랜 기다림 불만을 표출하는 애니메이션 실행
	if(bCheckPlayAnim != true && curTime > 2)
	{
		bCheckPlayAnim = true;
		
		int32 temp = SetRandRange(0, 1);

		// 랜덤으로 두가지 애니메이션 중에 하나 실행
		if(temp == 0)
		{
			owner->customerAnim->OnSitAnim(TEXT("WaitLong1"));
		}
		else if(temp == 1)
		{
			owner->customerAnim->OnSitAnim(TEXT("WaitLong2"));
		}
	}
	
	// 코스터가 있으면
	if(spawnManager->bIsCoaster[idx] != false && curTime > 3)
	{
		owner->order_UI->EndCustomer();
		
		SetSitState(ECustomerSitState::ORDER);
	}
}

void UCustomerFSM::TickOrder()
{
	if(bCheckPlayAnim != true && curTime > 1)
	{
		bCheckPlayAnim = true;

		owner->order_UI->SetSwitcher(1);
		owner->order_UI->SetOrderText(owner->voiceCount, orderIdx);
		owner->order_UI->StartCustomer();
		
		// 주문하는 애니메이션 실행
		owner->customerAnim->OnSitAnim(TEXT("Order"));
		
		spawnManager->aChairs[idx]->bOnceOverlap = false;
	}
}

void UCustomerFSM::TickWait()
{
	VisibleOrder();

	// 일정 시간안에 코스터와 칵테일이 준비 되면 상태 이동
	if(curTime < 10 && spawnManager->bIsCoaster[idx] != false && spawnManager->bIsCoctail[idx] != false)
	{
		owner->order_UI->EndCustomer();

		SetSitState(ECustomerSitState::ORDERJUDGE);
	}
	// 그렇지 않다면 불만표시로 상태 이동
	else if(curTime >= 10)
	{
		SetSitState(ECustomerSitState::WAITLONG);
	}
}

void UCustomerFSM::TickWaitLong()
{
	VisibleOrder();

	// 오랜 기다림 불만을 표출하는 애니메이션 실행
	if(bCheckPlayAnim != true && curTime > 2)
	{
		bCheckPlayAnim = true;
		
		int32 temp = SetRandRange(0, 1);

		// 랜덤으로 두가지 애니메이션 중에 하나 실행
		if(temp == 0)
		{
			owner->customerAnim->OnSitAnim(TEXT("WaitLong1"));
		}
		else if(temp == 1)
		{
			owner->customerAnim->OnSitAnim(TEXT("WaitLong2"));
		}
	}
	
	// 코스터와 칵테일이 준비 되면 상태 이동
	if(spawnManager->bIsCoaster[idx] != false && spawnManager->bIsCoctail[idx] != false)
	{
		owner->order_UI->EndCustomer();

		SetSitState(ECustomerSitState::ORDERJUDGE);
	}
}

void UCustomerFSM::TickOrderJudge()
{
	// 주문과 일치하면 holdcup으로 상태 이동
	if(spawnManager->aChairs[idx]->bSameOrder == true)
	{
		if(curTime > 2 && player->isDropped == true)
		{
			SetSitState(ECustomerSitState::HOLDCUP);

			spawnManager->aChairs[idx]->MoveCup();
		}
	}
	// 주문과 일치 하지 않고
	else if(spawnManager->aChairs[idx]->bUnSameOrder == true)
	{
		// 한번 더 기회를 소모하지 않았으면
		if(bCheckOrder != true && curTime > 1)
		{
			if(bCheckCustomer != true)
			{
				owner->order_UI->SetSwitcher(1);
				owner->order_UI->SetOrderTwiceText(owner->voiceCount, orderIdx);
				owner->order_UI->StartCustomer();

				bCheckCustomer = true;
			}
			
			// 화내는 애니메이션 실행
			SetSitState(ECustomerSitState::ANGRY);
			
			// 컵을 치우라는 알림 띄우기
			spawnManager->aChairs[idx]->ViewInfo();
			
			// 컵이 없다면
			if(spawnManager->bIsCoctail[idx] != true)
			{
				bCheckOrder = true;

				spawnManager->aChairs[idx]->bOnceOverlap = false;

				spawnManager->aChairs[idx]->HideScore();

				owner->order_UI->EndCustomer();
				
				SetSitState(ECustomerSitState::ORDER);
			}
		}
		// 한번의 기회를 소모했으면
		else if(bCheckOrder != false && curTime > 1)
		{
			if(bCheckCustomer != true && curTime > 2 && curTime <= 4)
			{
				owner->order_UI->SetSwitcher(1);
				owner->order_UI->SetLeaveText(owner->voiceCount);
				owner->order_UI->StartCustomer();

				bCheckCustomer = true;
			}

			if(bCheckCustomer != false && curTime > 4)
			{
				owner->order_UI->EndCustomer();

				bCheckCustomer = true;
				
				SetSitState(ECustomerSitState::ANGRY);
			}
		}
	}
}

void UCustomerFSM::TickHoldCup()
{
	if(bCheckPlayAnim != true && curTime > 2)
	{
		bCheckPlayAnim = true;

		// 컵을 잡는 애니메이션 실행
		owner->customerAnim->OnSitAnim(TEXT("HoldCup"));
	}
}

void UCustomerFSM::TickDrink()
{
	switch (drinkState)
	{
	case ECustomerDrinkState::IDLE:
		TickIdleCup();
		break;
	case ECustomerDrinkState::DRINK:
		TickDrinkCup();
		break;
	case ECustomerDrinkState::UNHOLDCUP:
		TickUnHoldCup();
		break;
	}
}

void UCustomerFSM::TickTasteJudge()
{
	// 점수 판단
	auto result = spawnManager->aChairs[idx]->totalScore;

	if(result >= 40)
	{
		SetSitState(ECustomerSitState::AWESOME);
	}
	else
	{
		SetSitState(ECustomerSitState::ANGRY);
	}
}

void UCustomerFSM::TickAngry()
{
	// 주문한 음료가 제대로 나왔을 경우
	if(spawnManager->aChairs[idx]->bSameOrder == true)
	{
		if(bCheckPlayAnim != true)
		{
			owner->order_UI->SetSwitcher(1);
			owner->order_UI->SetBadText(owner->voiceCount);
			owner->order_UI->StartCustomer();
			
			bCheckPlayAnim = true;

			// 화내는 애니메이션 실행
			owner->customerAnim->OnSitAnim(TEXT("Angry"));
		}
	}
	// 주문한 음료가 제대로 안나왔을 경우
	else if(spawnManager->aChairs[idx]->bUnSameOrder == true)
	{
		if(bCheckPlayAnim != true)
		{
			bCheckPlayAnim = true;

			// 화내는 애니메이션 실행
			owner->customerAnim->OnSitAnim(TEXT("Angry"));
		}
	}
}

void UCustomerFSM::TickAwesome()
{
	if(bCheckPlayAnim != true)
	{
		owner->order_UI->SetSwitcher(1);
		owner->order_UI->SetGoodText(owner->voiceCount);
		owner->order_UI->StartCustomer();
		
		bCheckPlayAnim = true;

		// 좋아하는 애니메이션 실행
		owner->customerAnim->OnSitAnim(TEXT("Good"));
	}
}

void UCustomerFSM::TickReadyLeave()
{
	if(spawnManager->aChairs[idx]->bSameOrder == true)
	{
		if(bCheckCustomer != true && curTime > 1 && curTime <= 3)
		{
			bCheckCustomer = true;

			owner->order_UI->EndCustomer();
		}
		if(bCheckCustomer != false && curTime > 3 && curTime <= 5)
		{
			bCheckCustomer = false;

			float money = 5 + spawnManager->aChairs[idx]->totalScore/10;
				
			spawnManager->gi->AddMoney(money);
				
			owner->order_UI->SetMoneyText(money);
			owner->order_UI->SetSwitcher(2);
			owner->order_UI->StartCustomer();

			UGameplayStatics::PlaySound2D(GetWorld(), owner->cashSound);
		}

		if(bCheckPlayAnim != true && curTime > 5)
		{
			owner->order_UI->EndCustomer();
			
			DetachCustomer();

			// 컵을 치우라는 알림 띄우기
			spawnManager->aChairs[idx]->ViewInfo();
			
			bCheckPlayAnim = true;

			// 자리에서 일어나는 애니메이션 실행
			owner->customerAnim->OnSitAnim(TEXT("LeaveSit"));
		}
	}
	else if(spawnManager->aChairs[idx]->bUnSameOrder == true)
	{
		if(bCheckPlayAnim != true)
		{
			DetachCustomer();

			// 컵을 치우라는 알림 띄우기
			spawnManager->aChairs[idx]->ViewInfo();
			
			bCheckPlayAnim = true;

			// 자리에서 일어나는 애니메이션 실행
			owner->customerAnim->OnSitAnim(TEXT("LeaveSit"));
		}
	}
}

// -----------------------------------------------------------------------------------------drink
// drink 상태 설정 함수
void UCustomerFSM::SetDrinkState(ECustomerDrinkState next)
{
	// 상태 변경
	drinkState = next;

	// 상태가 변경 될 때마다 대기시간 초기화
	curTime = 0;

	// 플레이 되는 애니메이션 플레이 체크 초기화
	bCheckPlayAnim = false;

	bCheckCustomer = false;
}

void UCustomerFSM::TickIdleCup()
{
	if(curTime > 1 && bCheckPlayAnim != true)
	{
		spawnManager->aChairs[idx]->HideScore();
		
		bCheckPlayAnim = true;

		// 잔을 들고 cheers 애니메이션 실행
		owner->customerAnim->OnDrinkAnim(TEXT("Cheers"));
	}
}

void UCustomerFSM::TickDrinkCup()
{
	if(drinkCount < 4)
	{
		auto tempIdx = SetRandRange(1, 4);

		// 3번 마시게 하고 애니메이션은 랜덤으로 한다
		if(tempIdx > 3 && curTime > 2)
		{
			if(bCheckPlayAnim != true)
			{
				bCheckPlayAnim = true;

				drinkCount++;
				
				owner->customerAnim->OnDrinkAnim(TEXT("DrinkAll"));
			}
		}
		else if(tempIdx > 0 && curTime > 2)
		{
			if(bCheckPlayAnim != true)
			{
				bCheckPlayAnim = true;

				drinkCount++;
				
				owner->customerAnim->OnDrinkAnim(TEXT("DrinkLittle"));
			}
		}
	}
	else
	{
		if(owner->cup != nullptr)
		{
			owner->cup->contents = 0;

			owner->cup->liquorComp->SetVisibility(false);

			if(owner->cup->halfSlicedLimeRef)
			{
				owner->cup->halfSlicedLimeRef->Destroy();
			}
			if(owner->cup->slicedLimeRef)
			{
				owner->cup->slicedLimeRef->Destroy();
			}
			if(owner->cup->halfSlicedOrangeRef)
			{
				owner->cup->halfSlicedOrangeRef->Destroy();
			}
			if(owner->cup->slicedOrangeRef)
			{
				owner->cup->slicedOrangeRef->Destroy();
			}
			if(owner->cup->oliveRef)
			{
				owner->cup->oliveRef->Destroy();
			}
		}
		
		SetDrinkState(ECustomerDrinkState::UNHOLDCUP);
	}
}

void UCustomerFSM::TickUnHoldCup()
{
	if(bCheckPlayAnim != true)
	{
		bCheckPlayAnim = true;

		// 컵 놓는 애니메이션 실행
		owner->customerAnim->OnDrinkAnim(TEXT("UnHoldCup"));
	}
}

// 오더위젯 보이게 하는 함수
void UCustomerFSM::VisibleOrder()
{
	if(spawnManager->bIsPlayer[idx] != false)
	{
		// 플레이어가 근처에 있으면 주문하는 대사 보여주기
		owner->order_UI->SetSwitcher(1);
	}
	else
	{
		// 플레이어가 근터에 없으면 물음표
		owner->order_UI->SetImage(owner->order_UI->iconImage[1]);

		owner->order_UI->SetSwitcher(0);
	}
}

// 랜덤 함수
int32 UCustomerFSM::SetRandRange(int32 idxStart, int32 idxEnd)
{
	int32 result = FMath::RandRange(idxStart, idxEnd);

	return result;
}