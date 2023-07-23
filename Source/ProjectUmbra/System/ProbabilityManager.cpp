// Fill out your copyright notice in the Description page of Project Settings.


#include "ProbabilityManager.h"

#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "ProjectUmbra/CustomProjectSettings/ProjectUmbraSettings.h"
#include "ProjectUmbra/Items/HealthPack.h"
#include "ProjectUmbra/Items/TagDrop.h"

// Sets default values
UProbabilityManager::UProbabilityManager()
{
	m_TCurrentTagDrops = TArray<ATagDrop*>();
	m_TCurrentHealthDrops = TArray<AHealthPack*>();
	m_APlayerPtr = nullptr;
}

void UProbabilityManager::Init(UProjectUmbraSettings* _UProjectSettings,
	UProjectUmbraGameInstance* _UProjectGameInstance)
{
	m_UProjectSettings = _UProjectSettings;
	m_UProjectGameInstance = _UProjectGameInstance;

	m_fBaseProbability = m_UProjectSettings->m_fBaseProbability;
	m_fProbabilityReductionPerFloorTag = m_UProjectSettings->m_fBaseProbability;
	m_fProbabilityReductionIfPlayerHasTag = m_UProjectSettings->m_fProbabilityReductionIfPlayerHasTag;

	m_fProbabilityAt1Health = m_UProjectSettings->m_fProbabilityAt1Health;
	m_fProbabilityAt2Health = m_UProjectSettings->m_fProbabilityAt2Health;
	m_fProbabilityAt3Health = m_UProjectSettings->m_fProbabilityAt3Health;
	m_fProbabilityAt4Health = m_UProjectSettings->m_fProbabilityAt4Health;
	m_fProbabilityAt5Health = m_UProjectSettings->m_fProbabilityAt5Health;
}

bool UProbabilityManager::CanSpawnTagDrop()
{
	float fBaseProbability = m_fBaseProbability;

	for(int32 i = 0; i < m_TCurrentTagDrops.Num(); ++i)
	{
		if(!IsValid(m_TCurrentTagDrops[i]))
		{
			m_TCurrentTagDrops.RemoveAt(i);
			--i;
		}
	}
	
	fBaseProbability -= m_TCurrentTagDrops.Num() * m_fProbabilityReductionPerFloorTag;

	if(m_APlayerPtr)
	{
		if(m_APlayerPtr->m_eTag != EBulletModifier::None)
			fBaseProbability -= m_fProbabilityReductionIfPlayerHasTag;
	}
	
	const float fDiceValue = FMath::FRand();
	
	return fDiceValue < fBaseProbability;
}

bool UProbabilityManager::CanSpawnHealthDrop()
{
	for(int32 i = 0; i < m_TCurrentHealthDrops.Num(); ++i)
	{
		if(!IsValid(m_TCurrentHealthDrops[i]))
		{
			m_TCurrentHealthDrops.RemoveAt(i);
			--i;
		}
	}
	
	//TODO: Cambiar cuando se tenga la formula final
	if(m_APlayerPtr)
	{
		const float fCurrentHealth = m_APlayerPtr->mc_UHealth->GetHealth() + m_TCurrentHealthDrops.Num();
		const float fMaxHealth = m_APlayerPtr->GetHealthComponent()->m_fMaxHealth;

		float fRand = FMath::FRand();

		if(fCurrentHealth == 1.0f)
		{
			return fRand < m_fProbabilityAt1Health;
		}

		if(fCurrentHealth == 2.0f)
		{
			return fRand < m_fProbabilityAt2Health;
		}

		if(fCurrentHealth == 3.0f)
		{
			return fRand < m_fProbabilityAt3Health;
		}

		if(fCurrentHealth == 4.0f)
		{
			return fRand < m_fProbabilityAt4Health;
		}
	
		if(fCurrentHealth == fMaxHealth)
		{
			return fRand < m_fProbabilityAt5Health;
		}

		return false;
	}
	
	return false;
}

void UProbabilityManager::RegisterTagDrop(ATagDrop* _ADropToRegister)
{
	m_TCurrentTagDrops.Add(_ADropToRegister);
}

void UProbabilityManager::UnRegisterTagDrop(ATagDrop* _ADropToUnRegister)
{
	m_TCurrentTagDrops.Remove(_ADropToUnRegister);
}

void UProbabilityManager::RegisterHealthDrop(AHealthPack* _ADropToRegister)
{
	m_TCurrentHealthDrops.Add(_ADropToRegister);
}

void UProbabilityManager::UnRegisterHealthDrop(AHealthPack* _ADropToUnRegister)
{
	m_TCurrentHealthDrops.Remove(_ADropToUnRegister);
}

void UProbabilityManager::SetPlayer(APlayerCharacter* _APlayerPtr)
{
	m_APlayerPtr = _APlayerPtr;
}

