// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

#include <Components/TextBlock.h>
#include <Components/WidgetComponent.h>

#include "BarGameInstance.h"
#include "MenuWidgetActor.h"
#include "PlayerCharacter.h"
#include "TutorialCheckBox.h"
#include "TutorialWidget.h"
#include "XRLoadingScreenFunctionLibrary.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	menuActor = Cast<AMenuWidgetActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AMenuWidgetActor::StaticClass()));

	gi = Cast<UBarGameInstance>(GetGameInstance());

	// 0일차 메뉴 버튼 바인드
	btn_StoryMode->OnPressed.AddDynamic(this, &UMenuWidget::StoryStart);
	btn_ArcadeMode->OnPressed.AddDynamic(this, &UMenuWidget::ArcadeStart);
	btn_QuitGame->OnPressed.AddDynamic(this, &UMenuWidget::ChoiceQuit);

	// 튜토리얼 선택
	btn_TutorialYes->OnPressed.AddDynamic(this, &UMenuWidget::TutorialMode);
	btn_TutorialNo->OnPressed.AddDynamic(this, &UMenuWidget::NoTutorialMode);

	// 이후 메뉴 버튼 바인드
	btn_ResumeGame->OnPressed.AddDynamic(this, &UMenuWidget::ResumeGame);
	// 체크포인트 바인드
	btn_CheckPoint->OnPressed.AddDynamic(this, &UMenuWidget::ChoiceCheck);
	btn_CheckPointYes->OnPressed.AddDynamic(this, &UMenuWidget::CheckYes);
	btn_CheckPointNo->OnPressed.AddDynamic(this, &UMenuWidget::CheckNo);
	// 메인메뉴 바인드
	btn_ResetGame->OnPressed.AddDynamic(this, &UMenuWidget::ChoiceReset);
	btn_MainYes->OnPressed.AddDynamic(this, &UMenuWidget::ResetYes);
	btn_MainNo->OnPressed.AddDynamic(this, &UMenuWidget::ResetNo);
	// 게임종료 바인드
	btn_QuitGame1->OnPressed.AddDynamic(this, &UMenuWidget::ChoiceQuit);
	btn_QuitYes->OnPressed.AddDynamic(this, &UMenuWidget::QuitGameYes);
	btn_QuitNo->OnPressed.AddDynamic(this, &UMenuWidget::QuitGameNo);
	// 언어 선택 바인드
	btn_SettingsMain->OnPressed.AddDynamic(this, &UMenuWidget::LanguageSettings);
	btn_Settings->OnPressed.AddDynamic(this, &UMenuWidget::LanguageSettings);
	btn_SetEnglish->OnPressed.AddDynamic(this, &UMenuWidget::EnglishSelected);
	btn_SetKorean->OnPressed.AddDynamic(this, &UMenuWidget::KoreanSelected);
	btn_LanguageCheckYes->OnPressed.AddDynamic(this, &UMenuWidget::LangCheckYes);
	btn_LanguageCheckNo->OnPressed.AddDynamic(this, &UMenuWidget::LangCheckNo);
	// 핸드 리셋 바인드
	btn_ResetHands->OnPressed.AddDynamic(this, &UMenuWidget::ChoiceResetHand);
	btn_HandResetCheckYes->OnPressed.AddDynamic(this, &UMenuWidget::HandResetYes);
	btn_HandResetCheckNo->OnPressed.AddDynamic(this, &UMenuWidget::HandResetNo);
}

void UMenuWidget::StoryStart()
{
	WidgetSwitcher_Menu->SetActiveWidgetIndex(1);
}

void UMenuWidget::ArcadeStart()
{
	WidgetSwitcher_Menu->SetActiveWidgetIndex(1);
	
	if(gi != nullptr)
	{
		gi->bCheckArcadeMode = true;
	}
}

void UMenuWidget::ChoiceQuit()
{
	WidgetSwitcher_Menu->SetActiveWidgetIndex(5);
}

void UMenuWidget::NoTutorialMode()
{
	if(gi != nullptr)
	{
		gi->bCheckTutorialMode = true;
	}

	auto parent = Cast<AMenuWidgetActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AMenuWidgetActor::StaticClass()));

	if(parent != nullptr)
	{
		gi->bCheckMenu = true;
		
		parent->Destroy();
	}

	auto player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if(player != nullptr)
	{
		player->PlayerMenuText();
	}
}

void UMenuWidget::TutorialMode()
{
	if(gi != nullptr)
	{
		gi->bCheckTutorialMode = false;
	}

	auto parent = Cast<AMenuWidgetActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AMenuWidgetActor::StaticClass()));

	if(parent != nullptr)
	{
		gi->bCheckMenu = true;
		
		parent->Destroy();
	}

	auto player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if(player != nullptr)
	{
		player->PlayerMenuText();
	}
}

void UMenuWidget::QuitGameYes()
{
	APlayerController* palyer = GetWorld()->GetFirstPlayerController();
	UKismetSystemLibrary::QuitGame(GetWorld(), palyer, EQuitPreference::Quit, true);
}

void UMenuWidget::QuitGameNo()
{
	if(gi->bCheckMenu != true)
	{
		WidgetSwitcher_Menu->SetActiveWidgetIndex(0);
	}
	else
	{
		WidgetSwitcher_Menu->SetActiveWidgetIndex(2);
	}
}

void UMenuWidget::ResumeGame()
{
	auto parent = Cast<AMenuWidgetActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AMenuWidgetActor::StaticClass()));

	auto player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	
	if(parent != nullptr && player != nullptr)
	{
		player->menuWidgetBool = false;
		
		parent->Destroy();
	}
}

void UMenuWidget::ChoiceCheck()
{
	WidgetSwitcher_Menu->SetActiveWidgetIndex(3);
}

void UMenuWidget::CheckYes()
{
	auto currentMap = UGameplayStatics::GetCurrentLevelName(GetWorld());

	if(gi->bCheckArcadeMode != true)
	{
		// 체크포인트로 보내기
		if(gi->checkDayCount == 0 && currentMap != "")
		{
			if(currentMap == "BarStartMap")
			{
				gi->bCheckTutorialMode = false;

				gi->bCheckMenu = false;

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();

				UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
			}
			else if(currentMap == "BarTutorialMap")
			{
				gi->bCheckTutorialMode = false;

				gi->bCheckMenu = true;

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();

				UGameplayStatics::OpenLevel(GetWorld(), "BarTutorialMap");
			}
		}
		else if(gi->checkDayCount == 1 && currentMap != "")
		{
			if(currentMap == "BarStartMap")
			{
				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
			}
			else if(currentMap == "BarMainMap")
			{
				gi->TotalMoney = {0, 0, 0, 0};

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarMainMap");
			}
		}
		else if(gi->checkDayCount == 2 && currentMap != "")
		{
			if(currentMap == "BarStartMap")
			{
				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
			}
			else if(currentMap == "BarMainMap")
			{
				auto oneday = gi->TotalMoney[0];

				auto twoday = gi->TotalMoney[1];
				
				auto total = gi->TotalMoney[3];

				auto settotal = total - twoday;
				
				gi->TotalMoney = {oneday, 0, 0, settotal};
				
				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarMainMap");
			}
		}
		else if(gi->checkDayCount == 3 && currentMap != "")
		{
			if(currentMap == "BarStartMap")
			{
				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
			}
			else if(currentMap == "BarMainMap")
			{
				auto oneday = gi->TotalMoney[0];

				auto twoday = gi->TotalMoney[1];
				
				auto threeday = gi->TotalMoney[2];
				
				auto total = gi->TotalMoney[3];

				auto settotal = total - threeday;
				
				gi->TotalMoney = {oneday, twoday, 0, settotal};

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarMainMap");
			}
		}
	}
	else
	{
		// 체크포인트로 보내기
		if(gi->checkDayCount == 0 && currentMap != "")
		{
			if(currentMap == "BarStartMap")
			{
				gi->bCheckTutorialMode = false;

				gi->bCheckArcadeMode = false;
				
				gi->bCheckMenu = false;

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();

				UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
			}
			else if(currentMap == "BarTutorialMap")
			{
				gi->bCheckTutorialMode = false;

				gi->bCheckMenu = true;

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();

				UGameplayStatics::OpenLevel(GetWorld(), "BarTutorialMap");
			}
		}
		else if(gi->checkDayCount == 3 && currentMap != "")
		{
			if(currentMap == "BarStartMap")
			{
				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
			}
			else if(currentMap == "BarMainMap")
			{
				gi->TotalMoney = {0, 0, 0, 0};

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarMainMap");
			}
		}
	}
}

void UMenuWidget::CheckNo()
{
	WidgetSwitcher_Menu->SetActiveWidgetIndex(2);
}

void UMenuWidget::LangCheckYes()
{
	if(bSelectEnglish)
	{
		SetLanguageEnglish();
		bSelectEnglish=false;
	}
	if(bSelectKorean)
	{
		SetLanguageKorean();
		bSelectKorean=false;
	}	
		auto currentMap = UGameplayStatics::GetCurrentLevelName(GetWorld());

	if(gi->bCheckArcadeMode != true)
	{
		// 체크포인트로 보내기
		if(gi->checkDayCount == 0 && currentMap != "")
		{
			if(currentMap == "BarStartMap")
			{
				gi->bCheckTutorialMode = false;

				gi->bCheckMenu = false;

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();

				UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
			}
			else if(currentMap == "BarTutorialMap")
			{
				gi->bCheckTutorialMode = false;

				gi->bCheckMenu = true;

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();

				UGameplayStatics::OpenLevel(GetWorld(), "BarTutorialMap");
			}
		}
		else if(gi->checkDayCount == 1 && currentMap != "")
		{
			if(currentMap == "BarStartMap")
			{
				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
			}
			else if(currentMap == "BarMainMap")
			{
				gi->TotalMoney = {0, 0, 0, 0};

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarMainMap");
			}
		}
		else if(gi->checkDayCount == 2 && currentMap != "")
		{
			if(currentMap == "BarStartMap")
			{
				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
			}
			else if(currentMap == "BarMainMap")
			{
				auto oneday = gi->TotalMoney[0];

				auto twoday = gi->TotalMoney[1];
				
				auto total = gi->TotalMoney[3];

				auto settotal = total - twoday;
				
				gi->TotalMoney = {oneday, 0, 0, settotal};
				
				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarMainMap");
			}
		}
		else if(gi->checkDayCount == 3 && currentMap != "")
		{
			if(currentMap == "BarStartMap")
			{
				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
			}
			else if(currentMap == "BarMainMap")
			{
				auto oneday = gi->TotalMoney[0];

				auto twoday = gi->TotalMoney[1];
				
				auto threeday = gi->TotalMoney[2];
				
				auto total = gi->TotalMoney[3];

				auto settotal = total - threeday;
				
				gi->TotalMoney = {oneday, twoday, 0, settotal};

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarMainMap");
			}
		}
	}
	else
	{
		// 체크포인트로 보내기
		if(gi->checkDayCount == 0 && currentMap != "")
		{
			if(currentMap == "BarStartMap")
			{
				gi->bCheckTutorialMode = false;

				gi->bCheckArcadeMode = false;
				
				gi->bCheckMenu = false;

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();

				UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
			}
			else if(currentMap == "BarTutorialMap")
			{
				gi->bCheckTutorialMode = false;

				gi->bCheckMenu = true;

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();

				UGameplayStatics::OpenLevel(GetWorld(), "BarTutorialMap");
			}
		}
		else if(gi->checkDayCount == 3 && currentMap != "")
		{
			if(currentMap == "BarStartMap")
			{
				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
			}
			else if(currentMap == "BarMainMap")
			{
				gi->TotalMoney = {0, 0, 0, 0};

				UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
				UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
				UGameplayStatics::OpenLevel(GetWorld(), "BarMainMap");
			}
		}
	}
}

void UMenuWidget::LangCheckNo()
{
	bSelectEnglish=false;
	bSelectKorean=false;
	if(gi)
	{
		if(gi->bCheckMenu != true)
		{
			WidgetSwitcher_Menu->SetActiveWidgetIndex(0);
		}
		else
		{
			WidgetSwitcher_Menu->SetActiveWidgetIndex(2);
		}
	}
}

void UMenuWidget::ChoiceReset()
{
	WidgetSwitcher_Menu->SetActiveWidgetIndex(4);
}

void UMenuWidget::ResetYes()
{
	gi->checkDayCount = 0;

	gi->bCheckTutorialMode = false;

	gi->bCheckArcadeMode = false;
	
	gi->bCheckMenu = false;

	gi->TotalMoney = {0, 0, 0, 0};

	UXRLoadingScreenFunctionLibrary::SetLoadingScreen(LoadingTexture, FVector2D(1, 1), FVector(1, 0, 1.5), true, false);
	UXRLoadingScreenFunctionLibrary::ShowLoadingScreen();
	UGameplayStatics::OpenLevel(GetWorld(), "BarStartMap");
}

void UMenuWidget::ResetNo()
{
	WidgetSwitcher_Menu->SetActiveWidgetIndex(2);
}

void UMenuWidget::LanguageSettings()
{
	WidgetSwitcher_Menu->SetActiveWidgetIndex(6);
}

void UMenuWidget::SetLanguageEnglish()
{
	if(gi)
	{
		gi->bEnglish=true;
	}
	text_CurLang->SetText(FText::FromString("Current Language : English"));
	text_AskTutorial->SetText(FText::FromString("Do you want to play the tutorial?"));
	text_TutYes->SetText(FText::FromString("Yes"));
	text_TutNo->SetText(FText::FromString("No"));
	text_AskCheckpoint->SetText(FText::FromString("Do you want to return to the checkpoint?"));
	text_CheckYes->SetText(FText::FromString("Yes"));
	text_CheckNo->SetText(FText::FromString("No"));
	text_AskBackToMain->SetText(FText::FromString("Do you want to return to the main menu?"));
	text_AskYes->SetText(FText::FromString("Yes"));
	text_AskNo->SetText(FText::FromString("No"));
	text_AskQuitGame->SetText(FText::FromString("Do you want to exit the game?"));
	text_QuitYes->SetText(FText::FromString("Yes"));
	text_QuitNo->SetText(FText::FromString("No"));
	text_AskLanguage->SetText(FText::FromString("Select Language"));
	text_LangEng->SetText(FText::FromString("English"));
	text_LangKor->SetText(FText::FromString("Korean"));
	text_LanguageCheck1->SetText(FText::FromString("Set up the language, you will return to the checkpoint."));
	text_LanguageCheck2->SetText(FText::FromString("Would you like to return to the checkpoint now?"));
	text_LanguageCheckYes->SetText(FText::FromString("Yes"));
	text_LanguageCheckNo->SetText(FText::FromString("No"));
	text_HandResetCheck->SetText(FText::FromString("You can reset your hands in case of trouble."));
	text_HandResetCheck2->SetText(FText::FromString("Do you want to reset hands now?"));
	text_HandResetCheckYes->SetText(FText::FromString("Yes"));
	text_HandResetCheckNo->SetText(FText::FromString("No"));


	if(gi)
	{
		if(gi->bCheckMenu != true)
		{
			WidgetSwitcher_Menu->SetActiveWidgetIndex(0);
		}
		else
		{
			WidgetSwitcher_Menu->SetActiveWidgetIndex(2);
		}
	}
	
}

void UMenuWidget::SetLanguageKorean()
{
	if(gi)
	{
		gi->bEnglish=false;
	}
	text_CurLang->SetText(FText::FromString("Current Language : Korean"));
	KorLanguageArray.Emplace(FString::Printf(TEXT("네"))); //0
	KorLanguageArray.Emplace(FString::Printf(TEXT("아니오"))); //1
	KorLanguageArray.Emplace(FString::Printf(TEXT("튜토리얼을 진행하시겠습니까?"))); //2
	KorLanguageArray.Emplace(FString::Printf(TEXT("영어 / English"))); //3
	KorLanguageArray.Emplace(FString::Printf(TEXT("한국어 / Korean"))); //4
	KorLanguageArray.Emplace(FString::Printf(TEXT("체크 포인트로 돌아가겠습니까?"))); //5
	KorLanguageArray.Emplace(FString::Printf(TEXT("메인 메뉴로 돌아가겠습니까?"))); //6
	KorLanguageArray.Emplace(FString::Printf(TEXT("게임을 종료하겠습니까?"))); //7
	KorLanguageArray.Emplace(FString::Printf(TEXT("언어를 선택하세요"))); //8
	KorLanguageArray.Emplace(FString::Printf(TEXT("언어 설정을 하면, 체크 포인트로 돌아가게 됩니다."))); //9
	KorLanguageArray.Emplace(FString::Printf(TEXT("지금 체크 포인트로 돌아가시겠습니까?"))); //10
	KorLanguageArray.Emplace(FString::Printf(TEXT("문제가 발생한 경우에, 손을 리셋할 수 있습니다."))); //11
	KorLanguageArray.Emplace(FString::Printf(TEXT("지금 손을 리셋하시겠습니까?"))); //12
	text_AskTutorial->SetText(FText::FromString(KorLanguageArray[2]));
	text_TutYes->SetText(FText::FromString(KorLanguageArray[0]));
	text_TutNo->SetText(FText::FromString(KorLanguageArray[1]));
	text_AskCheckpoint->SetText(FText::FromString(KorLanguageArray[5]));
	text_CheckYes->SetText(FText::FromString(KorLanguageArray[0]));
	text_CheckNo->SetText(FText::FromString(KorLanguageArray[1]));
	text_AskBackToMain->SetText(FText::FromString(KorLanguageArray[6]));
	text_AskYes->SetText(FText::FromString(KorLanguageArray[0]));
	text_AskNo->SetText(FText::FromString(KorLanguageArray[1]));
	text_AskQuitGame->SetText(FText::FromString(KorLanguageArray[7]));
	text_QuitYes->SetText(FText::FromString(KorLanguageArray[0]));
	text_QuitNo->SetText(FText::FromString(KorLanguageArray[1]));
	text_AskLanguage->SetText(FText::FromString(KorLanguageArray[8]));
	text_LangEng->SetText(FText::FromString(KorLanguageArray[3]));
	text_LangKor->SetText(FText::FromString(KorLanguageArray[4]));
	text_LanguageCheck1->SetText(FText::FromString(KorLanguageArray[9]));
	text_LanguageCheck2->SetText(FText::FromString(KorLanguageArray[10]));
	text_LanguageCheckYes->SetText(FText::FromString(KorLanguageArray[0]));
	text_LanguageCheckNo->SetText(FText::FromString(KorLanguageArray[1]));
	text_HandResetCheck->SetText(FText::FromString(KorLanguageArray[11]));
	text_HandResetCheck2->SetText(FText::FromString(KorLanguageArray[12]));
	text_HandResetCheckYes->SetText(FText::FromString(KorLanguageArray[0]));
	text_HandResetCheckNo->SetText(FText::FromString(KorLanguageArray[1]));
	
	if(gi)
	{
		if(gi->bCheckMenu != true)
		{
			WidgetSwitcher_Menu->SetActiveWidgetIndex(0);
		}
		else
		{
			WidgetSwitcher_Menu->SetActiveWidgetIndex(2);
		}
	}
	

}

void UMenuWidget::ChoiceResetHand()
{
	WidgetSwitcher_Menu->SetActiveWidgetIndex(8);
}

void UMenuWidget::HandResetYes()
{
	if(gi)
	{
		gi->resetHandDele.Broadcast();
	}

	ResumeGame();
}

void UMenuWidget::HandResetNo()
{
	WidgetSwitcher_Menu->SetActiveWidgetIndex(2);
}

void UMenuWidget::EnglishSelected()
{
	bSelectEnglish=true;
	WidgetSwitcher_Menu->SetActiveWidgetIndex(7);
}

void UMenuWidget::KoreanSelected()
{
	bSelectKorean=true;
	WidgetSwitcher_Menu->SetActiveWidgetIndex(7);
}
