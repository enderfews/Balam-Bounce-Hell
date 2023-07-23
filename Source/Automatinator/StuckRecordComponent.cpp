// Fill out your copyright notice in the Description page of Project Settings.


#include "Automatinator/StuckRecordComponent.h"
#include "TimerManager.h"
#include "Misc/CString.h"
#include "HAL/FileManager.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Misc/DateTime.h"
#include "Misc/Paths.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Automatinator/StuckRecordManager.h"

// Sets default values for this component's properties
UStuckRecordComponent::UStuckRecordComponent():m_fRecordRate(1.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStuckRecordComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	GetWorld()->GetTimerManager().SetTimer(m_Timer, this, &UStuckRecordComponent::Record, m_fRecordRate, true);

	AActor* m_pActor=UGameplayStatics::GetActorOfClass(GetWorld(), AStuckRecordManager::StaticClass());
	m_pManager = Cast<AStuckRecordManager>(m_pActor);

}


void UStuckRecordComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/*FString FileName = "StuckResult_";
	FileName.Append(GetWorld()->GetMapName()).Append(TEXT("_"));
	FDateTime Date = FDateTime::Now();
	FString sDate, sTime;
	UKismetStringLibrary::Split(Date.ToString(), FString(TEXT("-")), sDate, sTime);
	FileName.Append(sDate);
	FileName.Append(TEXT(".txt"));
	TArray<FString> Results;
	FString Line;
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FString Dir = FPaths::ProjectContentDir().Append("/Files/");
	FString FileDir = Dir.Append(FileName);
	if (!FileManager.FileExists(*FileDir)) {


		for (auto It = m_StuckCountMap.CreateConstIterator(); It; ++It) {
			Line = It.Key().ToString().Append(TEXT("||")).Append(FString::FromInt(It.Value()));
			Results.Add(Line);
		}

		if (FFileHelper::SaveStringArrayToFile(Results, *FileDir))
		{
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Sucsesfuly Written: \"%d\" FStrings to the text file"), Results.Num());

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Could not save the stuck results, something wrong :("));
		}
	}
	else {
		TArray<FString> ExistingLines;
		FFileHelper::LoadFileToStringArray(ExistingLines, *FileName);
		int prevLines = ExistingLines.Num();
		for (auto It = m_StuckCountMap.CreateConstIterator(); It; ++It) {
			Line = It.Key().ToString().Append(TEXT("||")).Append(FString::FromInt(It.Value()));
			ExistingLines.Add(Line);
		}
		if (FFileHelper::SaveStringArrayToFile(ExistingLines, *FileDir))
		{
			int newLinesNum = ExistingLines.Num() - prevLines;
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Sucsesfuly Modified: added \"%d\" new FStrings to the text file"), newLinesNum);

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Could not modify the stuck results, something wrong :("));
		}
	}*/
}

// Called every frame
void UStuckRecordComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStuckRecordComponent::Record()
{
	if (m_pManager) {
		UE_LOG(LogTemp, Warning, TEXT("Recording data for stuck"));
		FVector Loc = UKismetMathLibrary::Vector_SnappedToGrid(GetOwner()->GetActorLocation(), 300.f);
		if (m_pManager->m_StuckCountMap.Contains(Loc)) {
			m_pManager->m_StuckCountMap[Loc] += 1;
		}
		else {
			m_pManager->m_StuckCountMap.Add(Loc, 1);
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Could not find the stuck manager reference. Something went wrong :("));
	}
}

