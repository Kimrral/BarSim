// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class BARSIM_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class AMenuWidgetActor* menuActor;

	UPROPERTY()
	class ATutorialCheckBox* tutoCheckBox;
	
	UPROPERTY()
	class UTutorialWidget* tutoWidget;

	UPROPERTY()
	class UBarGameInstance* gi;
	
	// 위젯스위처
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_Menu;

	// 메뉴 버튼
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_StoryMode;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_ArcadeMode;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_QuitGame;

	// 모드 선택
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_TutorialYes;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_TutorialNo;

	// 메뉴 스폰
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_ResumeGame;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_CheckPoint;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_ResetGame;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_QuitGame1;

	// 체크포인트 선택
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_CheckPointYes;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_CheckPointNo;
	
	// 메인메뉴 선택
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_MainYes;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_MainNo;
	
	// 게임종료 선택
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_QuitYes;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_QuitNo;

	// 언어 선택
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_SettingsMain;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_Settings;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_SetEnglish;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_SetKorean;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_LanguageCheckYes;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_LanguageCheckNo;

	// 손 리셋
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_ResetHands;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_HandResetCheckYes;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_HandResetCheckNo;
	
	
	// 0일차 메뉴 함수
	UFUNCTION()
	void StoryStart();

	UFUNCTION()
	void ArcadeStart();
	
	UFUNCTION()
	void ChoiceQuit();
	
	UFUNCTION()
	void NoTutorialMode();

	UFUNCTION()
	void TutorialMode();
	
	UFUNCTION()
	void QuitGameYes();

	UFUNCTION()
	void QuitGameNo();

	// 이후 메뉴 함수
	UFUNCTION()
	void ResumeGame();

	UFUNCTION()
	void ChoiceCheck();

	UFUNCTION()
	void CheckYes();

	UFUNCTION()
	void CheckNo();

	UFUNCTION()
	void LangCheckYes();

	UFUNCTION()
	void LangCheckNo();

	UFUNCTION()
	void ChoiceReset();
	
	UFUNCTION()
	void ResetYes();

	UFUNCTION()
	void ResetNo();

	UFUNCTION()
	void LanguageSettings();

	UFUNCTION()
	void SetLanguageEnglish();

	UFUNCTION()
	void SetLanguageKorean();

	UFUNCTION()
	void ChoiceResetHand();

	UFUNCTION()
	void HandResetYes();

	UFUNCTION()
	void HandResetNo();


	UPROPERTY()
	TArray<FString> KorLanguageArray;

	UPROPERTY(EditAnywhere, Category="Texture")
	class UTexture* LoadingTexture;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_AskTutorial;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_TutYes;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_TutNo;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_AskCheckpoint;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_CheckYes;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_CheckNo;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_AskBackToMain;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_AskYes;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_AskNo;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_AskQuitGame;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_QuitYes;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_QuitNo;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_AskLanguage;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_LangEng;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_LangKor;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_CurLang;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_LanguageCheck1;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_LanguageCheck2;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_LanguageCheckYes;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_LanguageCheckNo;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_HandResetCheck;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_HandResetCheck2;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_HandResetCheckYes;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* text_HandResetCheckNo;

	UPROPERTY()
	bool bSelectEnglish=false;

	UPROPERTY()
	bool bSelectKorean=false;

	UFUNCTION()
	void EnglishSelected();

	UFUNCTION()
	void KoreanSelected();
};
