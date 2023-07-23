// Fill out your copyright notice in the Description page of Project Settings.


#include "TxtFileAssetFactory.h"
#include "Misc/FeedbackContext.h"
#include "Automatinator/TxtFileAsset.h"
#include "Misc/DateTime.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"

UTxtFileAssetFactory::UTxtFileAssetFactory()
{
	SupportedClass = UTxtFileAsset::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
	bEditAfterNew = true;
	//Format;Format description
	Formats.Add("txt;txt file where results will be recorded");
}

UObject* UTxtFileAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UTxtFileAsset* Asset= NewObject<UTxtFileAsset>(InParent, InClass, InName, Flags);
	TArray<FString> FileLines;
	if (Asset != nullptr)
	{
		IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
		if (FileManager.FileExists(*Filename)) {
			FFileHelper::LoadFileToStringArray(FileLines, *Filename);
			Asset->m_sFileDir = Filename;
			Asset->m_TLines = FileLines;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Could not find txt file, something wrong :("));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not create the object, something wrong :("));
	}
	return Asset;
}

