// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPack.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "ProjectUmbra/System/ProbabilityManager.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"

// Sets default values
AHealthPack::AHealthPack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mc_UPivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	SetRootComponent(mc_UPivot);
	mc_UTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	mc_UTrigger->SetupAttachment(mc_UPivot);
	mc_UMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mc_UMesh->SetupAttachment(mc_UPivot);
}

void AHealthPack::OnOverlap(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor,
	UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex, bool _bFromSweep, const FHitResult& _FSweepResult)
{
	APlayerCharacter* APlayerChara = Cast<APlayerCharacter>(_AOtherActor);
	if(APlayerChara)
	{
		m_APlayerPtr = APlayerChara;
		m_bIsMoving = APlayerChara->GetHealthComponent()->GetHealth() < APlayerChara->GetHealthComponent()->m_fMaxHealth;

		if(m_bIsMoving)
		{
			m_fCurrentSpeed = m_fMagnetStartSpeed;
			SetActorEnableCollision(false);
			if(IsValid(m_UMeshMaterial))
			{
				m_UMeshMaterial->SetScalarParameterValue("BlinkFrequency", 0.0f);
			}
		}
	}
}

void AHealthPack::OnOverlapEnd(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor,
	UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex)
{
	APlayerCharacter* APlayerChara = Cast<APlayerCharacter>(_AOtherActor);
	if(APlayerChara && !m_bIsMoving)
	{
		m_APlayerPtr = nullptr;
	}
}

#if WITH_EDITOR
void AHealthPack::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(GET_MEMBER_NAME_CHECKED(AHealthPack, m_fMagnetRadius) == PropertyChangedEvent.GetPropertyName())
	{
		mc_UTrigger->SetSphereRadius(m_fMagnetRadius);
	}
}
#endif

// Called when the game starts or when spawned
void AHealthPack::BeginPlay()
{
	Super::BeginPlay();

	mc_UTrigger->OnComponentBeginOverlap.AddDynamic(this, &AHealthPack::OnOverlap);
	mc_UTrigger->OnComponentEndOverlap.AddDynamic(this, &AHealthPack::OnOverlapEnd);

	m_UMeshMaterial = mc_UMesh->CreateDynamicMaterialInstance(0, mc_UMesh->GetMaterial(0));
}

void AHealthPack::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetGameInstance<UProjectUmbraGameInstance>()->GetProbabilityManager()->UnRegisterHealthDrop(this);
}

// Called every frame
void AHealthPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(m_bIsMoving && m_APlayerPtr != nullptr)
	{
		if(m_fCurrentMagnetDuration >= m_fMagnetMaxDuration)
		{
			Destroy();
			return;
		}
		if(m_fMagnetMaxDuration - m_fCurrentMagnetDuration <= 3.0f)
		{
			if(IsValid(m_UMeshMaterial))
			{
				m_UMeshMaterial->SetScalarParameterValue("BlinkFrequency", 2.0f);
			}
		}
		
		const float fMeanSpeed = FMath::Min(m_fCurrentSpeed + m_fMagnetAcceleration * 0.5f * DeltaTime, m_fMagnetMaxSpeed);
		m_fCurrentSpeed += FMath::Min(m_fCurrentSpeed + m_fMagnetAcceleration  * DeltaTime, m_fMagnetMaxSpeed);

		const FVector VDirToPlayer = (m_APlayerPtr->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		const FVector VMeanVelocity = VDirToPlayer * fMeanSpeed;

		const FVector VNewLocation = GetActorLocation() + VMeanVelocity * DeltaTime;

		SetActorLocation(VNewLocation);

		if((m_APlayerPtr->GetActorLocation() - GetActorLocation()).SizeSquared() <= m_fDistanceToHeal * m_fDistanceToHeal)
		{
			m_APlayerPtr->mc_UHealth->HealDamage(m_fHealAmount);
			// UGameplayStatics::PlaySoundAtLocation(GetWorld(),m_UPickUpSound,GetActorLocation(),1,1);
			Destroy();
			return;
		}

		m_fCurrentMagnetDuration += DeltaTime;
	}
	else
	{
		if(m_APlayerPtr != nullptr)
		{
			if(m_APlayerPtr->GetHealthComponent()->GetHealth() < m_APlayerPtr->GetHealthComponent()->m_fMaxHealth)
			{
				m_bIsMoving = true;
				m_fCurrentSpeed = m_fMagnetStartSpeed;
				SetActorEnableCollision(false);
				if(IsValid(m_UMeshMaterial))
				{
					m_UMeshMaterial->SetScalarParameterValue("BlinkFrequency", 0.0f);
				}
				return;
			}
			const FVector VPlayerPosXY = FVector(m_APlayerPtr->GetActorLocation().X, m_APlayerPtr->GetActorLocation().Y, 0.0f);
			const FVector VHealLocationXY = FVector(GetActorLocation().X, GetActorLocation().Y, 0.0f);
			if((VPlayerPosXY - VHealLocationXY).SizeSquared() <= m_fDistanceToHeal * m_fDistanceToHeal){
				m_APlayerPtr->mc_UHealth->HealDamage(m_fHealAmount);
				Destroy();
				return;
			}
		}
		if(m_fDuration - m_fCurrentDuration <= 3.0f)
		{
			if(IsValid(m_UMeshMaterial))
			{
				m_UMeshMaterial->SetScalarParameterValue("BlinkFrequency", 2.0f);
			}
		}
		if(m_fCurrentDuration >= m_fDuration)
		{
			Destroy();
		}
		m_fCurrentDuration += DeltaTime;
	}
}

