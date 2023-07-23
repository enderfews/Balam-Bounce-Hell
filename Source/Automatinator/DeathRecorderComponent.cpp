// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathRecorderComponent.h"
#include "Misc/CString.h"
#include "HAL/FileManager.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Misc/DateTime.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
//#include "ProjectUmbra/System/UmbraGameMode.h"
// Sets default values for this component's properties
UDeathRecorderComponent::UDeathRecorderComponent() :m_TDeathMap(TMap<FVector, int>()), m_bActivateRecorder(true)//, m_pGameMode(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDeathRecorderComponent::BeginPlay()
{
	Super::BeginPlay();

	//m_pGameMode = GetWorld()->GetAuthGameMode<AUmbraGameMode>();
	//if (m_pGameMode)
	//{
	//	m_pGameMode->OnPlayerRespawned.AddDynamic(this, &UDeathRecorderComponent::RecordOnDeath);
	//}
	//else
	//{
	//	#ifdef WITH_EDITOR
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in DeathRecordercomponent: Could not cast to Umbra gamemode"));
	//	#endif 
	//	UE_LOG(LogTemp, Error, TEXT("Error in DeathRecorderComponent: Could not cast to Umbra gamemode"));
	//}

}
void UDeathRecorderComponent::SaveDeaths()
{
	if (m_bActivateRecorder && m_TDeathMap.Num()!=0)
	{
		FString FileName = "DeathResult_";
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
		for (auto It = m_TDeathMap.CreateConstIterator(); It; ++It)
		{
			Line = It.Key().ToString().Append(TEXT("||")).Append(FString::FromInt(It.Value()));
			TResults.Add(Line);
		}
		if (FFileHelper::SaveStringArrayToFile(TResults, *FileDir))
		{
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Sucsesfuly Written: \"%d\" FStrings to the text file"), TResults.Num());
			UE_LOG(LogTemp, Warning, TEXT("Saved at: %s"), *FileDir);
			m_TDeathMap.Empty();
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Could not save the results, something wrong :("));
		}
	}
}
void UDeathRecorderComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SaveDeaths();
}


// Called every frame
void UDeathRecorderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDeathRecorderComponent::RecordOnDeath()
{
	if (m_bActivateRecorder)
	{
		UE_LOG(LogTemp, Warning, TEXT("Recorded player death data"));
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Orange, TEXT("Recorded player death data"));
		FVector Loc = UKismetMathLibrary::Vector_SnappedToGrid(GetOwner()->GetActorLocation(), 300.f);
		if (m_TDeathMap.Contains(Loc))
		{
			m_TDeathMap[Loc] += 1;
		}
		else {
			m_TDeathMap.Add(Loc, 1);
		}
	}
}

