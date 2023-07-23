// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionRecorderComponent.h"
#include "Misc/CString.h"
#include "HAL/FileManager.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Misc/DateTime.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
// Sets default values for this component's properties
UActionRecorderComponent::UActionRecorderComponent():m_TCutMap(TMap<FVector,int>()), m_TSpecialMap(TMap<FVector, int>()),m_bActivateRecorder(true)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UActionRecorderComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UActionRecorderComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (m_bActivateRecorder)
	{

		if (m_TCutMap.Num() != 0)
		{
			SaveCutRecords();
		}
		if (m_TSpecialMap.Num() != 0)
		{
			SaveSpecialRecords();
		}
	}
}


// Called every frame
void UActionRecorderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UActionRecorderComponent::Record(EPlayerActionType eType)
{
	switch (eType)
	{
	case EPlayerActionType::Cut:
		RecordOnCut();
		break;
	case EPlayerActionType::Special:
		RecordOnSpecial();
		break;
	case EPlayerActionType::Bat:
		//Implement bat record if design desire it
		break;
	case EPlayerActionType::Dash:
		//Implement dash record if design desire it
		break;
	default:
		break;
	}
}
void UActionRecorderComponent::RecordOnCut()
{
	if (m_bActivateRecorder)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Orange, TEXT("Recorded player death data"));
		FVector Loc = UKismetMathLibrary::Vector_SnappedToGrid(GetOwner()->GetActorLocation(), 300.f);
		if (m_TCutMap.Contains(Loc))
		{
			m_TCutMap[Loc] += 1;
		}
		else {
			m_TCutMap.Add(Loc, 1);
		}
	}
}

void UActionRecorderComponent::RecordOnSpecial()
{
	if (m_bActivateRecorder)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Orange, TEXT("Recorded player death data"));
		FVector Loc = UKismetMathLibrary::Vector_SnappedToGrid(GetOwner()->GetActorLocation(), 300.f);
		if (m_TSpecialMap.Contains(Loc))
		{
			m_TSpecialMap[Loc] += 1;
		}
		else {
			m_TSpecialMap.Add(Loc, 1);
		}
	}
}

void UActionRecorderComponent::SaveCutRecords()
{
	if (m_bActivateRecorder)
	{
		FString FileName = "CutResult_";
		FileName.Append(m_sCurrentLevelName).Append(TEXT("_"));
		FDateTime Date = FDateTime::Now();
		FString sDate, sTime;
		UKismetStringLibrary::Split(Date.ToString(), FString(TEXT("-")), sDate, sTime);
		FileName.Append(sDate);
		FileName.Append(TEXT(".txt"));
		TArray<FString> TResults;
		FString Line;
		IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
		FString Dir = FString(FPlatformProcess::BaseDir());
		FString FileDir = Dir.Append(FileName);
		for (auto It = m_TCutMap.CreateConstIterator(); It; ++It)
		{
			Line = It.Key().ToString().Append(TEXT("||")).Append(FString::FromInt(It.Value()));
			TResults.Add(Line);
		}
		if (FFileHelper::SaveStringArrayToFile(TResults, *FileDir))
		{
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Sucsesfuly Written: \"%d\" FStrings to the text cut file"), TResults.Num());
			UE_LOG(LogTemp, Warning, TEXT("Saved at: %s"), *FileDir);
			m_TCutMap.Empty();
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Could not save the results of cut, something wrong :("));
		}
	}
}

void UActionRecorderComponent::SaveSpecialRecords()
{
	if (m_bActivateRecorder)
	{
		FString FileName = "SpecialResult_";
		FileName.Append(m_sCurrentLevelName).Append(TEXT("_"));
		FDateTime Date = FDateTime::Now();
		FString sDate, sTime;
		UKismetStringLibrary::Split(Date.ToString(), FString(TEXT("-")), sDate, sTime);
		FileName.Append(sDate);
		FileName.Append(TEXT(".txt"));
		TArray<FString> TResults;
		FString Line;
		IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
		FString Dir = FString(FPlatformProcess::BaseDir());
		FString FileDir = Dir.Append(FileName);
		for (auto It = m_TSpecialMap.CreateConstIterator(); It; ++It)
		{
			Line = It.Key().ToString().Append(TEXT("||")).Append(FString::FromInt(It.Value()));
			TResults.Add(Line);
		}
		if (FFileHelper::SaveStringArrayToFile(TResults, *FileDir))
		{
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Sucsesfuly Written: \"%d\" FStrings to the text special file"), TResults.Num());
			UE_LOG(LogTemp, Warning, TEXT("Saved at: %s"), *FileDir);
			m_TSpecialMap.Empty();

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Could not save the results of special, something wrong :("));
		}
	}
}

