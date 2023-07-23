#include "TagDrop.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "ProjectUmbra/System/ProbabilityManager.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "ProjectUmbra/System/SoundManager.h"

ATagDrop::ATagDrop()
{
	PrimaryActorTick.bCanEverTick = true;
	
	mc_UPivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	SetRootComponent(mc_UPivot);
	
	mc_UCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	mc_UCollider->SetupAttachment(mc_UPivot);
}

void ATagDrop::BeginPlay()
{
	Super::BeginPlay();

	mc_UCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
}

void ATagDrop::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetGameInstance<UProjectUmbraGameInstance>()->GetProbabilityManager()->UnRegisterTagDrop(this);
}

void ATagDrop::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(m_bInteracted)
	{
		return;
	}

	if(m_fLifetime <= 0)
	{
		Destroy();
		return;
	}

	m_fLifetime -= DeltaSeconds;

	if(m_fTimeUntilActive > 0.0f)
	{
		m_fTimeUntilActive -= DeltaSeconds;
	}
	else if(!m_bActive)
	{
		m_bActive = true;
		mc_UCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	
}

void ATagDrop::Collect(AActor* _ACollector)
{
	m_bInteracted = true;
	APlayerCharacter* APlayerChara = Cast<APlayerCharacter>(_ACollector);

	if(APlayerChara != nullptr)
	{
		APlayerChara->ChangeTag(m_ETag);
	}
}

void ATagDrop::GetHit_Implementation(FVector _FHitDirection, AActor* _ANewOwner, bool _bPlayerOwnership,
	float _fAutoAimRange, float _fAutoAimAngle)
{
	if(_bPlayerOwnership && m_bActive && GetOwner() != _ANewOwner)
	{
		m_USndManager->PlayEventAtLocation(GetWorld(), m_UBatFMODEvent, GetActorLocation());
		SetOwner(_ANewOwner);
		Collect(_ANewOwner);
		OnBatHit();
	}
}
