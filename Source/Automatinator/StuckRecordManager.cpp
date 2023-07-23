// Fill out your copyright notice in the Description page of Project Settings.


#include "Automatinator/StuckRecordManager.h"
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

// Sets default values
AStuckRecordManager::AStuckRecordManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStuckRecordManager::BeginPlay()
{
	Super::BeginPlay();
	
}
void AStuckRecordManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FString FileName = "StuckResult_";
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

// Called every frame
void AStuckRecordManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


