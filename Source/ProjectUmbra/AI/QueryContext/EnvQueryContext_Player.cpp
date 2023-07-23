// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/QueryContext/EnvQueryContext_Player.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	
	APlayerCharacter* pPlayer = nullptr;
	TArray<AActor*> TChars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), TChars);
	/*for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if(Cast<APlayerCharacter>(*It))
		pPlayer = Cast<APlayerCharacter>(*It);
		break;
	}*/
	pPlayer = TChars.Num()!=0 ? Cast<APlayerCharacter>(TChars[0]) : nullptr;
	if (pPlayer == nullptr)
	{
		return;
	}
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, pPlayer);
}