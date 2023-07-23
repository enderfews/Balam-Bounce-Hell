#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "EnemyDeathDetectorComponent.generated.h"

UCLASS(ClassGroup=Detectors, meta=(BlueprintSpawnableComponent))
class PROJECTUMBRA_API UEnemyDeathDetectorComponent : public UActorComponent 
{
	GENERATED_BODY()
	
private:
	uint32 m_uEnemyNumber = 0;

	UFUNCTION()
	void EnemyDeath();

protected:
	virtual void BeginPlay() override;
	
public:
	UEnemyDeathDetectorComponent();
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Config", DisplayName="Observed Enemies")
	TArray<ABaseEnemy*> m_TObservedEnemies = TArray<ABaseEnemy*>();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnEnemyDeath );
	UPROPERTY(BlueprintAssignable)
	FOnEnemyDeath OnEnemyDeath;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnAllEnemiesDeath );
	UPROPERTY(BlueprintAssignable)
	FOnAllEnemiesDeath OnAllEnemiesDeath;
};
