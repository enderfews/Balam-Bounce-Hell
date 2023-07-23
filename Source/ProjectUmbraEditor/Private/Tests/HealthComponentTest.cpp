// Fill out your copyright notice in the Description page of Project Settings.


#include "Tests/HealthComponentTest.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Tests/AutomationEditorCommon.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHealthComponentTest,"Project.UnitTests.HealthComponentUnitTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)


//FEngineAutomationTestUtilities::LoadMap(MapName); <- DEPRECATED
//UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();
//TestEquals(message,Current,Expected)
//AddError(message)
bool FHealthComponentTest::RunTest(const FString& parameters)
{
	UWorld* pWorld = FAutomationEditorCommonUtils::CreateNewMap();
	bool test1=true,test2=true,test3=true,test4=true;
	{
		APlayerCharacter* pPlayer = pWorld->SpawnActor<APlayerCharacter>();
		test1=TestEqual(TEXT("Newly-spawned player character must start at max health"), pPlayer->mc_UHealth->GetHealth(), pPlayer->mc_UHealth->m_fMaxHealth);
		pPlayer->Destroy();
	}
	{
		APlayerCharacter* pPlayer = pWorld->SpawnActor<APlayerCharacter>();
		pPlayer->mc_UHealth->TakeDamage(1.f);
		test2 = TestNotEqual(TEXT("Damaged player character must loose an amount of health"), pPlayer->mc_UHealth->GetHealth(), pPlayer->mc_UHealth->m_fMaxHealth);
		pPlayer->Destroy();
	}
	{
		APlayerCharacter* pPlayer = pWorld->SpawnActor<APlayerCharacter>();
		pPlayer->mc_UHealth->SetHealth(1.f);
		float fCurrentHealth = pPlayer->mc_UHealth->GetHealth();
		pPlayer->mc_UHealth->HealDamage(1.f);
		test3 = TestNotEqual(TEXT("Healed player character must have more health"), pPlayer->mc_UHealth->GetHealth(), fCurrentHealth);
		pPlayer->Destroy();
	}
	{
		APlayerCharacter* pPlayer = pWorld->SpawnActor<APlayerCharacter>();
		pPlayer->mc_UHealth->SetHealth(1.f);
		pPlayer->mc_UHealth->HealDamage(10.f);
		test4 = TestEqual(TEXT("Healed player character must not have more health than max health"), pPlayer->mc_UHealth->GetHealth(), pPlayer->mc_UHealth->m_fMaxHealth);
		pPlayer->Destroy();
	}
	return test1 && test2 && test3 && test4;
}
