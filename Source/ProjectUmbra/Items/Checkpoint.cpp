// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Items/Checkpoint.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"
#include "Perception/AIPerceptionComponent.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "ProjectUmbra/System/UmbraGameMode.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PaperFlipbookComponent.h"
#include "Camera/CameraActor.h"
#include "Materials/Material.h"
#include "BrainComponent.h"
#include "AIController.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "ProjectUmbra/System/SoundManager.h"
#include "ProjectUmbra/Components/BorderHighlight.h"

// Sets default values
ACheckpoint::ACheckpoint(): AInteractable(),m_fMinHealth(1.f),m_fSavedHealth(0.f),m_bIsChecked(false), m_fMaxDistanceToRespawnInCheckpoint(1000.f),
m_fCheckpointRespawnTime(2.f), m_bScreenFade(true), m_bActivatesWithBat(true), m_bIsInvisible(false),m_TEnemies(TArray<FCheckpointData>()), m_bHeal(false), m_fHealingAmount(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mc_UBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	mc_UBody->SetupAttachment(mc_USphere);
	mc_UIcon = CreateDefaultSubobject<UWidgetComponent>(TEXT("Icon"));
	mc_UIcon->SetupAttachment(mc_USphere);
	mc_UFlipBook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipBook"));
	mc_UFlipBook->SetupAttachment(mc_USphere);
	mc_URespawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Respawn point"));
	mc_URespawnPoint->SetupAttachment(mc_USphere);
	mc_UBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	mc_UBox->SetupAttachment(mc_USphere);
	mc_UPlayerDetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Player detection box"));
	mc_UPlayerDetectionBox->SetupAttachment(mc_USphere);
	mc_UBorderHighlight = CreateDefaultSubobject<UBorderHighlight>(TEXT("Border high light component"));
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	//mc_UBox->UpdateOverlaps();
	mc_UPlayerDetectionBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnPlayerOverlap);
	TArray<AActor*> TToIgnore;
	TArray<AActor*> TActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> TQueries;
	TQueries.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TQueries.Add(EObjectTypeQuery::ObjectTypeQuery2);
	TQueries.Add(EObjectTypeQuery::ObjectTypeQuery3);
	TQueries.Add(EObjectTypeQuery::ObjectTypeQuery4);
	TQueries.Add(EObjectTypeQuery::ObjectTypeQuery5);
	TQueries.Add(EObjectTypeQuery::ObjectTypeQuery6);
	TQueries.Add(EObjectTypeQuery::ObjectTypeQuery7);
	TQueries.Add(EObjectTypeQuery::ObjectTypeQuery8);
	TQueries.Add(EObjectTypeQuery::ObjectTypeQuery9);
	TQueries.Add(EObjectTypeQuery::ObjectTypeQuery10);
	UKismetSystemLibrary::BoxOverlapActors(GetWorld(), mc_UBox->GetComponentLocation(), mc_UBox->GetScaledBoxExtent(),TQueries, nullptr, TToIgnore, TActors);
	for (AActor* pActor : TActors)
	{
		ABaseEnemy* pEnemy = Cast<ABaseEnemy>(pActor);
		if (pEnemy)
		{
			FCheckpointData Data;
			Data.m_UEnemyClass = GetEnemyClass(pEnemy->m_eEnemyType);
			Data.m_pEnemyRef = pEnemy;
			Data.m_vEnemyLoc = pEnemy->GetActorLocation();
			Data.m_rEnemyRot = pEnemy->GetActorRotation();
			Data.m_fEnemyHealth = pEnemy->mc_pHealthComp->m_fMaxHealth;
			m_TEnemies.Add(Data);
		}
	}
	AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (pGM)
	{
		pGM->m_TCheckpointRegister.Add(this);
	}
	else {
		#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in checkpoint: Could not cast to Umbra gamemode from checkpoint, something really bad could happen"));
		#endif 
		UE_LOG(LogTemp, Error, TEXT("Error in checkpoint: Could not cast to Umbra gamemode from checkpoint, something really bad could happen"));
	}
	//GetWorldTimerManager().SetTimer(m_FTimer, this, &ACheckpoint::CheckEnemies, 0.2f,true);

	m_USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
}

void ACheckpoint::CheckEnemies()
{
	if (m_TEnemies.Num() != 0)
	{
		for (int i=m_TEnemies.Num()-1;i>=0;--i)
		{
			if (!m_TEnemies[i].m_pEnemyRef || m_TEnemies[i].m_pEnemyRef->mc_pHealthComp->GetHealth() <= 0.f)
			{
				m_TEnemies.RemoveAt(i);
			}
		}
	}
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ACheckpoint::StopEnemiesLogic()
{
	ABaseEnemy* pEnemy = nullptr;
	ANormalEnemy* pNormal = nullptr;
	if (m_TEnemies.Num() != 0)
	{
		for (FCheckpointData& Data : m_TEnemies)
		{
			pEnemy = Data.m_pEnemyRef;
			if (pEnemy)
			{
				if (!pEnemy->IsPendingKill())
				{
					pNormal = Cast<ANormalEnemy>(pEnemy);
					if (pNormal)
						pNormal->m_bIsAbortedByCheckpoint = true;
					pEnemy->GetController<AAIController>()->GetBrainComponent()->StopLogic(TEXT(""));
					ANormalEnemyController* pController = pEnemy ? Cast<ANormalEnemyController>(pEnemy->GetController<AAIController>()) : nullptr;
					if (pController)
					{
						pController->mc_pAIPerception->ForgetAll();
						pController->mc_pAIPerception->SetSenseEnabled(UAISense_Sight::StaticClass(), false);
						pController->mc_pAIPerception->Deactivate();
						pController->mc_pBlackboard->ClearValue(FName("Player"));
					}
				}
			}
		}
	}
}
void ACheckpoint::ResetEnemies()
{
	ABaseEnemy* pEnemy = nullptr;
	ANormalEnemy* pNormal = nullptr;
	FActorSpawnParameters Params = FActorSpawnParameters();
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if (m_TEnemies.Num() != 0)
	{
		for (FCheckpointData& Data : m_TEnemies)
		{
			pEnemy = Data.m_pEnemyRef;
			if (pEnemy)
			{
				if (!pEnemy->IsPendingKill())
				{
					//pEnemy->Reset();
					pNormal = Cast<ANormalEnemy>(pEnemy);
					if (pNormal)
					{
						pNormal->m_bIsAbortedByCheckpoint = false;
						pNormal->GetCharacterMovement()->MaxWalkSpeed = pNormal->m_fPatrolSpeed;
					}
					pEnemy->GetController<AAIController>()->GetBrainComponent()->RestartLogic();
					pEnemy->SetActorLocation(Data.m_vEnemyLoc);
					pEnemy->SetActorRotation(Data.m_rEnemyRot);
					pEnemy->mc_pHealthComp->SetHealth(Data.m_fEnemyHealth);
					ANormalEnemyController* pController = pEnemy ? Cast<ANormalEnemyController>(pEnemy->GetController<AAIController>()) : nullptr;
					if (pController)
					{
						pController->mc_pAIPerception->Activate();
						pController->mc_pAIPerception->SetSenseEnabled(UAISense_Sight::StaticClass(), true);
					}
				}
				else
				{
					//Data.m_pEnemyRef = GetWorld()->SpawnActor<ABaseEnemy>(Data.m_UEnemyClass, Data.m_vEnemyLoc, Data.m_rEnemyRot, Params);
				}
			}else
			{
				//Data.m_pEnemyRef=GetWorld()->SpawnActor<ABaseEnemy>(Data.m_UEnemyClass, Data.m_vEnemyLoc, Data.m_rEnemyRot,Params);
			}
		}
	}
}

bool ACheckpoint::PlayerIsFarFromCheckpoint()
{
	if (GetOwner())
	{
		return (GetOwner()->GetActorLocation() - GetActorLocation()).Size() > m_fMaxDistanceToRespawnInCheckpoint;
	}
	else {
		return false;
	}
}

UClass* ACheckpoint::GetEnemyClass(EEnemyType _EnemyType)
{
	switch (_EnemyType)
	{
	case EEnemyType::Normal:
		return m_TNormalClass;
		break;
	case EEnemyType::Kamikaze:
		return m_TKamikazeClass;
		break;
	case EEnemyType::Circular:
		return m_THaloClass;
		break;
	case EEnemyType::Sniper:
		return m_TSniperClass;
		break;
	case EEnemyType::Shotgun:
		return m_TShotgunClass;
		break;
	case EEnemyType::Burst:
		return m_TBurstClass;
		break;
	default:
		return m_TNormalClass;
		break;
	}
	return nullptr;
}

void ACheckpoint::Interact(ACharacter* pCharacter)
{
}


void ACheckpoint::OnCheckpointOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ACheckpoint::OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!m_bActivatesWithBat)
	{
		APlayerCharacter* pPlayer = Cast<APlayerCharacter>(OtherActor);
		if (pPlayer && !m_bIsChecked)
		{
			AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
			if (OnHit.IsBound())
				OnHit.Broadcast();
			if (pGM)
			{
				if (pGM->m_pCheckpoint != this)
				{
					m_bIsChecked = true;
					if (pGM->m_pCheckpoint)
					{
						pGM->m_pCheckpoint->mc_UBody->SetMaterial(0, m_pInactiveMat);
						pGM->m_TCheckpointRegister.Remove(pGM->m_pCheckpoint);
					}
					SetOwner(OtherActor);
					pGM->m_pCheckpoint = this;
					pGM->m_vDefaultLoc = mc_URespawnPoint->GetComponentLocation();
					pGM->m_pCheckpoint->mc_UBody->SetMaterial(0, m_pActiveMat);
					pGM->m_fCurrentRespawnTime = this->m_fCheckpointRespawnTime;
					pGM->m_bCurrentScreenFade = this->m_bScreenFade;
					pGM->m_vCameraLoc = pPlayer->m_ACamera->GetActorLocation();
					m_fSavedHealth = UKismetMathLibrary::Clamp(pPlayer->mc_UHealth->GetHealth(), m_fMinHealth, 100.f);
					mc_UIcon->SetVisibility(false);
					mc_UFlipBook->SetVisibility(false);
					mc_UBorderHighlight->DisableBorderHighLight();

				}
			}
			else {
				#ifdef WITH_EDITOR
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in checkpoint: Could not cast to Umbra gamemode from checkpoint"));
				#endif 
				UE_LOG(LogTemp, Error, TEXT("Error in checkpoint: Could not cast to Umbra gamemode from checkpoint"));
			}
		}
	}
}

void ACheckpoint::GetHit_Implementation(FVector _vHitDirection, AActor* _ANewOwner, bool _bPlayerOwnership, float _fAutoAimRange, float _AutoAimAngle)
{
	if (m_bActivatesWithBat)
	{
		APlayerCharacter* pPlayer = Cast<APlayerCharacter>(_ANewOwner);
		if (pPlayer && !m_bIsChecked)
		{
			AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
			if (OnHit.IsBound())
				OnHit.Broadcast();
			
			m_USndManager->PlayEventAtLocation(GetWorld(), m_UBatFMODEvent, GetActorLocation());
			
			if (pGM)
			{
				if (pGM->m_pCheckpoint != this)
				{
					m_bIsChecked = true;
					if (pGM->m_pCheckpoint)
					{
						pGM->m_pCheckpoint->mc_UBody->SetMaterial(0, m_pInactiveMat);
						pGM->m_TCheckpointRegister.Remove(pGM->m_pCheckpoint);
					}
					SetOwner(_ANewOwner);
					pGM->m_pCheckpoint = this;
					pGM->m_vDefaultLoc = mc_URespawnPoint->GetComponentLocation();
					pGM->m_pCheckpoint->mc_UBody->SetMaterial(0, m_pActiveMat);
					pGM->m_fCurrentRespawnTime = this->m_fCheckpointRespawnTime;
					pGM->m_bCurrentScreenFade = this->m_bScreenFade;
					pGM->m_vCameraLoc = pPlayer->m_ACamera->GetActorLocation();

					if (m_bHeal)
					{
						pPlayer->mc_UHealth->HealDamage(m_fHealingAmount);
					}

					m_fSavedHealth = UKismetMathLibrary::Clamp(pPlayer->mc_UHealth->GetHealth(), m_fMinHealth, 100.f);
					mc_UIcon->SetVisibility(false);
					mc_UFlipBook->SetVisibility(false);
					mc_UBorderHighlight->DisableBorderHighLight();
				}
			}
			else {
				#ifdef WITH_EDITOR
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in checkpoint: Could not cast to Umbra gamemode from checkpoint"));
				#endif 
				UE_LOG(LogTemp, Error, TEXT("Error in checkpoint: Could not cast to Umbra gamemode from checkpoint"));
			}
		}
	}
}



