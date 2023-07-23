// Fill out your copyright notice in the Description page of Project Settings.


#include "Flamethrower.h"

#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "ProjectUmbra/System/SoundManager.h"

// Sets default values
AFlamethrower::AFlamethrower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mc_UPivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	SetRootComponent(mc_UPivot);
	mc_UWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall"));
	mc_UWall->SetupAttachment(mc_UPivot);
	mc_UFireStart = CreateDefaultSubobject<USceneComponent>(TEXT("Fire Start"));
	mc_UFireStart->SetupAttachment(mc_UPivot);
	mc_UDamageArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Area"));
	mc_UDamageArea->SetupAttachment(mc_UFireStart);
	mc_UMapCollisionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Map CollisionArea Area"));
	mc_UMapCollisionArea->SetupAttachment(mc_UFireStart);
	mc_UFlamethrowerEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Flamethrower Effect Component"));
	mc_UFlamethrowerEffectComponent->SetupAttachment(mc_UFireStart);
	
	mc_ULeftEyeEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Left Eye Effect Component"));
	mc_ULeftEyeEffectComponent->SetupAttachment(mc_UPivot);
	mc_URightEyeEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Righ tEye Effect Component"));
	mc_URightEyeEffectComponent->SetupAttachment(mc_UPivot);
}

void AFlamethrower::PlayerOverlap(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor,
	UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex, bool _bFromSweep, const FHitResult& _FSweepResult)
{
	TArray<AActor*> TActorsToIgnore;
	TActorsToIgnore.Add(_AOtherActor);
	TActorsToIgnore.Add(this);

	FHitResult FHitResult;
	
	if(UKismetSystemLibrary::BoxTraceSingle(
		GetWorld(),
		mc_UFireStart->GetComponentLocation(),
		FVector(_AOtherActor->GetActorLocation().X, _AOtherActor->GetActorLocation().Y, mc_UFireStart->GetComponentLocation().Z),
		FVector(1.0f,mc_UDamageArea->GetScaledBoxExtent().Y,mc_UDamageArea->GetScaledBoxExtent().Z),
		mc_UFireStart->GetComponentRotation(),
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TActorsToIgnore,
		EDrawDebugTrace::None,
		FHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.2f
		))
	{
		// UKismetSystemLibrary::PrintString(GetWorld(), FHitResult.Component->GetName());
		// UKismetSystemLibrary::PrintString(GetWorld(), FHitResult.Actor->GetName());
		return;
	}

	APlayerCharacter* APlayerChara = Cast<APlayerCharacter>(_AOtherActor);
	if(APlayerChara == nullptr)
	{
		return;
	}

	APlayerChara->ReceiveKnockback(GetKnockBackDir(APlayerChara),m_fKnockBackDuration, m_fKnockBackSpeed, m_fKnockBackStunDuration);
	UGameplayStatics::ApplyDamage(APlayerChara, m_fDamage, nullptr, this, nullptr);
}

void AFlamethrower::MapOverlap(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor,
	UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex, bool _bFromSweep, const FHitResult& _FSweepResult)
{
	CalculateEffectDistance();
}

void AFlamethrower::EndMapOverlap(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor,
	UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex)
{
	CalculateEffectDistance();
}

#if WITH_EDITOR
void AFlamethrower::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(GET_MEMBER_NAME_CHECKED(AFlamethrower, m_fLength) == PropertyChangedEvent.GetPropertyName())
	{
		m_fEffectLength = m_fLength;

		mc_UDamageArea->SetBoxExtent(FVector(m_fLength/2, mc_UDamageArea->GetScaledBoxExtent().Y, mc_UDamageArea->GetScaledBoxExtent().Z));
		mc_UDamageArea->SetRelativeLocation(FVector(m_fLength/2, 0.0f, 0.0f));

		mc_UMapCollisionArea->SetBoxExtent(FVector(m_fLength/2, mc_UMapCollisionArea->GetScaledBoxExtent().Y, mc_UMapCollisionArea->GetScaledBoxExtent().Z));
		mc_UMapCollisionArea->SetRelativeLocation(FVector(m_fLength/2, 0.0f, 0.0f));

		SetEffectDistance(m_fEffectLength);
	}
}
#endif

FVector AFlamethrower::GetKnockBackDir(const AActor* _APlayer) const
{
	FVector VFinalDir = GetActorRightVector();

	const FVector VPlayerPosXY = FVector(_APlayer->GetActorLocation().X, _APlayer->GetActorLocation().Y, 0.0f);
	const FVector VFlameThrowerPosXY = FVector(GetActorLocation().X, GetActorLocation().Y, 0.0f);
	
	const FVector VDamageToPlayer = (VPlayerPosXY - VFlameThrowerPosXY).GetSafeNormal();

	const float fSide = FVector::CrossProduct(GetActorForwardVector(), VDamageToPlayer).GetSafeNormal().Z;

	if(fSide != 0)
	{
		VFinalDir = GetActorRightVector() * fSide;
	}

	return VFinalDir;
}

void AFlamethrower::CalculateEffectDistance()
{
	TArray<AActor*> TActorsToIgnore;
	TActorsToIgnore.Add(this);

	FHitResult FHitResult;

	m_fEffectLength = m_fCurrentDistance;
	
	if(UKismetSystemLibrary::BoxTraceSingle(
		GetWorld(),
		mc_UFireStart->GetComponentLocation(),
		mc_UFireStart->GetComponentLocation() + GetActorForwardVector() * m_fEffectLength,
		FVector(1.0f,mc_UDamageArea->GetScaledBoxExtent().Y,mc_UDamageArea->GetScaledBoxExtent().Z),
		mc_UFireStart->GetForwardVector().Rotation(),
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TActorsToIgnore,
		EDrawDebugTrace::None,
		FHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.2f
		))
	{
		m_fEffectLength = FVector::Dist2D(FHitResult.Location,mc_UFireStart->GetComponentLocation());
	}
	
	SetEffectDistance(m_fEffectLength);
}

void AFlamethrower::SetEffectDistance(float _fDistance)
{
	mc_UFlamethrowerEffectComponent->SetFloatParameter("MinLifetime", _fDistance/1900.0f);
	mc_UFlamethrowerEffectComponent->SetFloatParameter("MaxLifetime", _fDistance/1600.0f);
}

// Called when the game starts or when spawned
void AFlamethrower::BeginPlay()
{
	Super::BeginPlay();

	m_fHalfWidth = mc_UDamageArea->GetScaledBoxExtent().Y;
	m_fHalfHeight = mc_UDamageArea->GetScaledBoxExtent().Z;
	m_fEffectLength = 0.0f;

	mc_UDamageArea->SetBoxExtent(FVector(0.0f, mc_UDamageArea->GetScaledBoxExtent().Y, mc_UDamageArea->GetScaledBoxExtent().Z));
	mc_UDamageArea->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	mc_UMapCollisionArea->SetBoxExtent(FVector(0.0f, mc_UMapCollisionArea->GetScaledBoxExtent().Y, mc_UMapCollisionArea->GetScaledBoxExtent().Z));
	mc_UMapCollisionArea->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	SetEffectDistance(m_fEffectLength);

	mc_UDamageArea->OnComponentBeginOverlap.AddDynamic(this, &AFlamethrower::PlayerOverlap);
	mc_UMapCollisionArea->OnComponentBeginOverlap.AddDynamic(this, &AFlamethrower::MapOverlap);
	mc_UMapCollisionArea->OnComponentEndOverlap.AddDynamic(this, &AFlamethrower::EndMapOverlap);
	
	m_bIsActive = m_bStartActive;
	mc_UFlamethrowerAudioComponent = GetGameInstance<UProjectUmbraGameInstance>()->
									GetSoundManager()->
									PlayEventAttached(m_UFlamethrowerFMODEvent, GetRootComponent(), FVector::ZeroVector,
									EAttachLocation::KeepRelativeOffset, true, true, false);

	mc_UFlamethrowerAudioComponent->SetParameter(FName("FlameThrowerIntensity"), 0.0f);
	if(m_bIsActive)
	{
		CalculateEffectDistance();
		mc_UFlamethrowerEffectComponent->Activate();
		mc_ULeftEyeEffectComponent->Activate(true);
		mc_URightEyeEffectComponent->Activate(true);
	} else
	{
		Stop();
	}

	
}

// Called every frame
void AFlamethrower::Tick(float _fDeltaTime)
{
	Super::Tick(_fDeltaTime);

	UKismetSystemLibrary::DrawDebugBox(
		GetWorld(),
		mc_UDamageArea->GetComponentLocation(),
		mc_UDamageArea->GetScaledBoxExtent(),
		m_bIsActive ? FColor::Green : FColor::Red,
		mc_UDamageArea->GetComponentRotation(),
		_fDeltaTime*2,
		2
		);
	
	if(m_bIsActive)
	{
		if(m_fCurrentDistance < m_fLength)
		{
			m_fCurrentDistance = FMath::Min(m_fCurrentDistance + _fDeltaTime * m_fProgressiveStartSpeed, m_fLength);
			mc_UDamageArea->SetBoxExtent(FVector(m_fCurrentDistance/2, mc_UDamageArea->GetScaledBoxExtent().Y, mc_UDamageArea->GetScaledBoxExtent().Z));
			mc_UDamageArea->SetRelativeLocation(FVector(m_fCurrentDistance/2, 0.0f, 0.0f));
			mc_UMapCollisionArea->SetBoxExtent(FVector(m_fCurrentDistance/2, mc_UMapCollisionArea->GetScaledBoxExtent().Y, mc_UMapCollisionArea->GetScaledBoxExtent().Z));
			mc_UMapCollisionArea->SetRelativeLocation(FVector(m_fCurrentDistance/2, 0.0f, 0.0f));
			mc_UFlamethrowerAudioComponent->SetParameter(FName("FlameThrowerIntensity"), m_fCurrentDistance / m_fLength);
			CalculateEffectDistance();
		} 
        if(m_fSleepTime <= 0.0f)
        {
            return;
        }
        
        m_fCurrentActiveTime += _fDeltaTime;

        if(m_fCurrentActiveTime >= m_fActiveTime)
        {
        	m_fCurrentFadeout = 1.0f;
            m_bIsActive = false;
            m_fCurrentActiveTime = 0.0f;
            m_fCurrentDistance = 0.0f;
            mc_UDamageArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        	mc_UDamageArea->SetBoxExtent(FVector(0.0f, mc_UDamageArea->GetScaledBoxExtent().Y, mc_UDamageArea->GetScaledBoxExtent().Z));
        	mc_UDamageArea->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        	mc_UMapCollisionArea->SetBoxExtent(FVector(0.0f, mc_UMapCollisionArea->GetScaledBoxExtent().Y, mc_UMapCollisionArea->GetScaledBoxExtent().Z));
        	mc_UMapCollisionArea->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        	
            mc_UFlamethrowerEffectComponent->Deactivate();
        }
	}
	else
	{
	    if(m_fSleepTime <= 0.0f)
		{
			return;
		}

		if(m_fCurrentFadeout > 0.0f)
		{
			m_fCurrentFadeout -= _fDeltaTime * m_fFadeoutSpeed;
			mc_UFlamethrowerAudioComponent->SetParameter(FName("FlameThrowerIntensity"), m_fCurrentFadeout);
		}
		
		m_fCurrentSleepTime += _fDeltaTime;
		m_fCurrentGlowEyesTime += _fDeltaTime;

		if(m_fCurrentGlowEyesTime >= m_fSleepTime + m_fTimeOffsetBeforeStart && !mc_ULeftEyeEffectComponent->IsActive())
		{
			mc_ULeftEyeEffectComponent->Activate(true);
		}

		if(m_fCurrentGlowEyesTime >= m_fSleepTime + m_fTimeOffsetBeforeStart && !mc_URightEyeEffectComponent->IsActive())
		{
			mc_URightEyeEffectComponent->Activate(true);
		}

		if(m_fCurrentSleepTime >= m_fSleepTime)
		{
			m_bIsActive = true;
			m_fCurrentSleepTime = 0.0f;
			m_fCurrentGlowEyesTime = 0.0f;
			mc_UDamageArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			mc_UDamageArea->SetBoxExtent(FVector(0.0f, mc_UDamageArea->GetScaledBoxExtent().Y, mc_UDamageArea->GetScaledBoxExtent().Z));
			mc_UDamageArea->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
			mc_UMapCollisionArea->SetBoxExtent(FVector(0.0f, mc_UMapCollisionArea->GetScaledBoxExtent().Y, mc_UMapCollisionArea->GetScaledBoxExtent().Z));
			mc_UMapCollisionArea->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

			mc_UFlamethrowerEffectComponent->Activate();
		}
	}
}

void AFlamethrower::Start()
{
	m_bIsActive = true;
	m_fCurrentSleepTime = 0.0f;
	m_fCurrentGlowEyesTime = 0.0f;
	m_fCurrentDistance = 0.0f;
	mc_UDamageArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	mc_UDamageArea->SetBoxExtent(FVector(0.0f, mc_UDamageArea->GetScaledBoxExtent().Y, mc_UDamageArea->GetScaledBoxExtent().Z));
	mc_UDamageArea->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	mc_UMapCollisionArea->SetBoxExtent(FVector(0.0f, mc_UMapCollisionArea->GetScaledBoxExtent().Y, mc_UMapCollisionArea->GetScaledBoxExtent().Z));
	mc_UMapCollisionArea->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	mc_UFlamethrowerEffectComponent->Activate();
	mc_UFlamethrowerAudioComponent->SetParameter(FName("FlameThrowerIntensity"), 0.0f);
	mc_ULeftEyeEffectComponent->Activate(true);
	mc_URightEyeEffectComponent->Activate(true);
	CalculateEffectDistance();
	SetActorTickEnabled(true);
}

void AFlamethrower::Stop()
{
	m_bIsActive = false;
	m_fCurrentActiveTime = 0.0f;
	m_fCurrentGlowEyesTime = 0.0f;
	m_fCurrentDistance = 0.0f;
	mc_UDamageArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mc_UDamageArea->SetBoxExtent(FVector(0.0f, mc_UDamageArea->GetScaledBoxExtent().Y, mc_UDamageArea->GetScaledBoxExtent().Z));
	mc_UDamageArea->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	mc_UMapCollisionArea->SetBoxExtent(FVector(0.0f, mc_UMapCollisionArea->GetScaledBoxExtent().Y, mc_UMapCollisionArea->GetScaledBoxExtent().Z));
	mc_UMapCollisionArea->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	mc_UFlamethrowerEffectComponent->Deactivate();
	mc_UFlamethrowerAudioComponent->SetParameter(FName("FlameThrowerIntensity"), 0.0f);
	SetActorTickEnabled(false);
}



