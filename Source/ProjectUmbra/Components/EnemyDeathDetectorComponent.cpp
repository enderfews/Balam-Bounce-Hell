#include "EnemyDeathDetectorComponent.h"

#include "HealthComponetCpp.h"

void UEnemyDeathDetectorComponent::EnemyDeath()
{
	--m_uEnemyNumber;
	if(OnEnemyDeath.IsBound())
	{
		OnEnemyDeath.Broadcast();
	}

	if(m_uEnemyNumber == 0 && OnAllEnemiesDeath.IsBound())
	{
		OnAllEnemiesDeath.Broadcast();
	}
}

void UEnemyDeathDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	for(const ABaseEnemy* AEnemy : m_TObservedEnemies)
	{
		if(IsValid(AEnemy))
		{
			++m_uEnemyNumber;
			AEnemy->mc_pHealthComp->OnDeath.AddDynamic(this, &UEnemyDeathDetectorComponent::EnemyDeath);
		}
	}
}

UEnemyDeathDetectorComponent::UEnemyDeathDetectorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
