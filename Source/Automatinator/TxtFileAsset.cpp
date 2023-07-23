// Fill out your copyright notice in the Description page of Project Settings.


#include "Automatinator/TxtFileAsset.h"
#include "Misc/DateTime.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
UTxtFileAsset::UTxtFileAsset()
{

}

void UTxtFileAsset::WriteLine(int i,const FString& _s)
{
	m_TLines[i] = _s;
}

void UTxtFileAsset::ReadLine(int i,FString& s_)
{
	s_ = m_TLines[i];
}

void UTxtFileAsset::AddChanges()
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	if (FileManager.FileExists(*m_sFileDir))
	{
		FFileHelper::SaveStringArrayToFile(m_TLines,*m_sFileDir);
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not save the results, something wrong :("));
	}
}
