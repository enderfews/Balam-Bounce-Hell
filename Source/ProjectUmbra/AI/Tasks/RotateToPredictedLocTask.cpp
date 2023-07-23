// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/RotateToPredictedLocTask.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectUmbra/Items/MovilePlatform.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

URotateToPredictedLocTask::URotateToPredictedLocTask()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type URotateToPredictedLocTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ANormalEnemyController* pController = Cast<ANormalEnemyController>(OwnerComp.GetOwner());
	ANormalEnemy* pEnemy = pController ? pController->m_pEnemyRef : nullptr;
	UBlackboardComponent* pBlackboard = pEnemy? OwnerComp.GetBlackboardComponent():nullptr;
	ACharacter* pEntityTarget = pBlackboard? Cast<ACharacter>(pBlackboard->GetValueAsObject(m_FEntityToFace.SelectedKeyName)):nullptr;
	if (pEntityTarget)
	{
		//Calculate the predicted position
		//float fDistance = (pEnemy->GetActorLocation() - pEntityTarget->GetActorLocation()).Size();
		//float fTravelTime = fDistance / pEnemy->m_fBulletSpeed;
		AMovilePlatform* pPlatform = Cast<AMovilePlatform>(pEntityTarget->GetAttachParentActor());
		FVector vPredictedLoc;
		if (pPlatform)
		{
			//FVector vPlatToEnemy = pEnemy->GetActorLocation() - pPlatform->mc_UPlatformMesh->GetComponentLocation();
			//float fD = vPlatToEnemy.Size();
			//float fSb = pEnemy->m_fBulletSpeed;
			//float fSt = (pPlatform->mc_UPlatformMesh->GetComponentVelocity() * 100.f).Size();
			//float fCos = UKismetMathLibrary::DegAcos(FVector::DotProduct(vPlatToEnemy, pPlatform->mc_UPlatformMesh->GetComponentVelocity()));
			//float fTimePos = (-2.f * fD * fSt * fCos + FMath::Sqrt(FMath::Pow(2.f * fD * fSt * fCos, 2) + 4.f * (FMath::Pow(fSb, 2) - FMath::Pow(fSt, 2)) * FMath::Pow(fD, 2)))/(2.f* (FMath::Pow(fSb, 2) - FMath::Pow(fSt, 2)));
			//float fTimeNeg = (-2.f * fD * fSt * fCos - FMath::Sqrt(FMath::Pow(2.f * fD * fSt * fCos, 2) + 4.f * (FMath::Pow(fSb, 2) - FMath::Pow(fSt, 2)) * FMath::Pow(fD, 2))) / (2.f * (FMath::Pow(fSb, 2) - FMath::Pow(fSt, 2)));
			//float fTime;
			//if (fTimePos > 0.f && fTimeNeg > 0.f)
			//{
			//	if (fTimePos < fTimeNeg)
			//	{
			//		fTime = fTimePos;
			//	}
			//	else {
			//		fTime = fTimeNeg;
			//	}
			//}
			//else if (fTimePos <= 0.f)
			//{
			//	fTime = fTimeNeg;
			//}
			//else {
			//	fTime = fTimePos;
			//}
			
			vPredictedLoc = pPlatform->mc_UPlatformMesh->GetComponentLocation();
			float fTime = 0.f;
			for (int i = 0; i < 4; ++i)
			{
				fTime = (vPredictedLoc - pEnemy->GetActorLocation()).Size() / pEnemy->m_fBulletSpeed;
				vPredictedLoc = pEntityTarget->GetActorLocation() + pPlatform->mc_UPlatformMesh->GetComponentVelocity() * fTime;
			}
			//UE_LOG(LogTemp, Error, TEXT("Platform velocity:%f"), pPlatform->mc_UPlatformMesh->GetComponentVelocity().Size());
			//vPredictedLoc = pEntityTarget->GetActorLocation() + pPlatform->mc_UPlatformMesh->GetComponentVelocity()*fTravelTime*80.f;
		}
		else {
			//FVector vPlayerToEnemy = pEnemy->GetActorLocation() - pEntityTarget->GetActorLocation();
			//float fD = vPlayerToEnemy.Size();
			//float fSb = pEnemy->m_fBulletSpeed;
			//float fSt = (pEntityTarget->GetCharacterMovement()->MaxWalkSpeed);
			//float fCos = UKismetMathLibrary::DegAcos(FVector::DotProduct(vPlayerToEnemy, pEntityTarget->GetRootComponent()->GetComponentVelocity()));
			//float fTimePos = (-2.f * fD * fSt * fCos + FMath::Sqrt(FMath::Pow(2.f * fD * fSt * fCos, 2) + 4.f * (FMath::Pow(fSb, 2) - FMath::Pow(fSt, 2)) * FMath::Pow(fD, 2))) / (2.f * (FMath::Pow(fSb, 2) - FMath::Pow(fSt, 2)));
			//float fTimeNeg = (-2.f * fD * fSt * fCos - FMath::Sqrt(FMath::Pow(2.f * fD * fSt * fCos, 2) + 4.f * (FMath::Pow(fSb, 2) - FMath::Pow(fSt, 2)) * FMath::Pow(fD, 2))) / (2.f * (FMath::Pow(fSb, 2) - FMath::Pow(fSt, 2)));
			//float fTime;
			//if (fTimePos > 0.f && fTimeNeg > 0.f)
			//{
			//	if (fTimePos < fTimeNeg)
			//	{
			//		fTime = fTimePos;
			//	}
			//	else {
			//		fTime = fTimeNeg;
			//	}
			//}
			//else if (fTimePos <= 0.f)
			//{
			//	fTime = fTimeNeg;
			//}
			//else {
			//	fTime = fTimePos;
			//}
			vPredictedLoc = pEntityTarget->GetActorLocation();
			float fTime = 0.f;
			for (int i = 0; i < 4; ++i)
			{
				fTime = (vPredictedLoc - pEnemy->GetActorLocation()).Size() / pEnemy->m_fBulletSpeed;
				vPredictedLoc = pEntityTarget->GetActorLocation() + pEntityTarget->GetRootComponent()->GetComponentVelocity() * fTime;
			}
			//vPredictedLoc = pEntityTarget->GetActorLocation() + pEntityTarget->GetRootComponent()->GetComponentVelocity() * fTravelTime;
		}
		//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), vPredictedLoc, 50.f, 12, FLinearColor::Red, 0.1f, 4.f);
		vPredictedLoc.Z = 0.f;
		//Perform the rotation
		FVector vEnemyLoc = pEnemy->GetActorLocation();
		vEnemyLoc.Z = 0.f;
		FRotator rTarget = UKismetMathLibrary::FindLookAtRotation(vEnemyLoc, vPredictedLoc);
		FRotator rCurrent = pEnemy->GetActorRotation();
		pEnemy->SmoothFacePlayer(rCurrent, rTarget);

		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}

FString URotateToPredictedLocTask::GetStaticDescription() const
{
	return FString("Perform a smooth rotation to face the predicted location");
}