// Fill out your copyright notice in the Description page of Project Settings.


#include "PathRecorderComponent.h"
#include "TimerManager.h"
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
UPathRecorderComponent::UPathRecorderComponent():m_TPathMap(TMap<FVector,int>()),m_fRecordRate(0.2f), m_bActivateRecorder(true)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPathRecorderComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if(m_bActivateRecorder)
		GetWorld()->GetTimerManager().SetTimer(m_FTimer, this, &UPathRecorderComponent::RecordPath,m_fRecordRate,true);
}
void UPathRecorderComponent::SavePaths()
{
	if (m_bActivateRecorder && m_TPathMap.Num() != 0)
	{
		FString FileName = "PathResult_";
		FileName.Append(m_sCurrentLevelName).Append(TEXT("_"));
		FDateTime Date = FDateTime::Now();
		FString sDate, sTime;
		UKismetStringLibrary::Split(Date.ToString(), FString(TEXT("-")), sDate, sTime);
		FileName.Append(sDate);
		FileName.Append(TEXT(".txt"));
		TArray<FString> TResults;
		FString Line;
		IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
		//FString Dir = FPaths::ProjectContentDir().Append("/Files/");
		//FString Dir = FPaths::LaunchDir().Append("/");
		//FString Dir = FPaths::ConvertRelativePathToFull(FPaths::RootDir());
		FString Dir = FString(FPlatformProcess::BaseDir());
		FString FileDir = Dir.Append(FileName);
		for (auto It = m_TPathMap.CreateConstIterator(); It; ++It)
		{
			Line = It.Key().ToString().Append(TEXT("||")).Append(FString::FromInt(It.Value()));
			TResults.Add(Line);
		}
		if (FFileHelper::SaveStringArrayToFile(TResults, *FileDir))
		{
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Sucsesfuly Written: \"%d\" FStrings to the text file"), TResults.Num());
			UE_LOG(LogTemp, Warning, TEXT("Saved at: %s"), *FileDir);
			m_TPathMap.Empty();
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Could not save the results, something wrong :("));
		}
	}
}
void UPathRecorderComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SavePaths();
}


// Called every frame
void UPathRecorderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPathRecorderComponent::RecordPath()
{
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Orange, TEXT("Recorded player path data"));
	FVector Loc = UKismetMathLibrary::Vector_SnappedToGrid(GetOwner()->GetActorLocation(), 100.f);
	if (m_TPathMap.Contains(Loc))
	{
		m_TPathMap[Loc] += 1;
	}
	else {
		m_TPathMap.Add(Loc, 1);
	}
}

