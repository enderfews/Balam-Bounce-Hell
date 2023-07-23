#include "ExplosiveEnemyChaseTask.h"

#include "ProjectUmbra/AI/Controllers/ExplosiveEnemyController.h"
#include "ProjectUmbra/Characters/ExplosiveEnemy.h"


UExplosiveEnemyChaseTask::UExplosiveEnemyChaseTask()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UExplosiveEnemyChaseTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	const AExplosiveEnemyController* pEnemyController = Cast<AExplosiveEnemyController>(OwnerComp.GetOwner());
	AExplosiveEnemy* pEnemy = pEnemyController ? pEnemyController->m_pExplosiveRef : nullptr;
	if (pEnemy)
	{
		pEnemy->EnableChaseSound(false);
		return EBTNodeResult::Aborted;
	}
	return EBTNodeResult::Aborted;
}
EBTNodeResult::Type UExplosiveEnemyChaseTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const AExplosiveEnemyController* pEnemyController = Cast<AExplosiveEnemyController>(OwnerComp.GetOwner());
	AExplosiveEnemy* pEnemy = pEnemyController ? pEnemyController->m_pExplosiveRef : nullptr;

	pEnemy->EnableChaseSound(true);
	
	return EBTNodeResult::InProgress;
}

FString UExplosiveEnemyChaseTask::GetStaticDescription() const
{
	return FString("This enables the chase sound");
}