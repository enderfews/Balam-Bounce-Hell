// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Items/MovilePlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SceneComponent.h"
#include "Curves/CurveFloat.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "ProjectUmbra/Components/EnemyDeathDetectorComponent.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "ProjectUmbra/System/SoundManager.h"


// Sets default values
AMovilePlatform::AMovilePlatform() :m_bSameArraySizes(false)
,m_bProvidedAllData(true),m_iIndex(1),m_FDstLoc(FVector(0.f,0.f,0.f)), m_vLastLoc(FVector::ZeroVector),m_FPlatformTimer(FTimerHandle()),m_bStartAgainSpline(false),
m_fTimeLineLength(1.f), m_bNeedsPlayer(false), m_bActivatesKillingEnemies(false), m_bTransitionBetweenPlatforms(false), m_fTimeToGetVel(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mc_URoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(mc_URoot);
	mc_UPlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	mc_UPlatformMesh->SetupAttachment(mc_URoot);
	mc_USpline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	mc_USpline->SetupAttachment(mc_URoot);
	mc_UBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	mc_UBox->OnComponentBeginOverlap.AddDynamic(this, &AMovilePlatform::OnOverlapBegin);
	mc_UBox->OnComponentEndOverlap.AddDynamic(this,&AMovilePlatform::OnOverlapEnd);
	mc_UBox->SetupAttachment(mc_UPlatformMesh);
	mc_UDeathDetector = CreateDefaultSubobject<UEnemyDeathDetectorComponent>(TEXT("Death detector"));
	mc_UDeathDetector->OnAllEnemiesDeath.AddDynamic(this, &AMovilePlatform::OnAllEnemiesDeathPlatform);
	static ConstructorHelpers::FObjectFinder<UCurveFloat> FindCurve(TEXT("CurveFloat'/Game/Blueprints/Curves/Platform_Curves/Platform_Curve.Platform_Curve'"));
	if (FindCurve.Succeeded())
	{
		m_UCurve = FindCurve.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR IN MOVILE PLATFORM: Could not find the curve for the spline, something went wrong :("));
	}
}

void AMovilePlatform::SplineTLFunc(float alpha)
{

	mc_UPlatformMesh->SetWorldLocation(mc_USpline->GetLocationAtDistanceAlongSpline(mc_USpline->GetSplineLength()*alpha,ESplineCoordinateSpace::Type::World),false,nullptr,ETeleportType::TeleportPhysics);
	m_fTimeToGetVel += GetWorld()->GetDeltaSeconds();
	//if (m_fTimeToGetVel >= 0.5f)
	//{
	//	m_fTimeToGetVel = 0.f;
		//FVector vVelocity = mc_UPlatformMesh->GetComponentLocation() - m_vLastLoc;
	    //m_vLastLoc = mc_UPlatformMesh->GetComponentLocation();
		//mc_UPlatformMesh->ComponentVelocity = vVelocity;
		mc_UPlatformMesh->ComponentVelocity = (mc_UPlatformMesh->GetComponentLocation() - m_vLastLoc) * (mc_USpline->GetSplineLength() / (m_fTimeLineLength * 10.f));
		m_vLastLoc = mc_UPlatformMesh->GetComponentLocation();
	//}
	//mc_UPlatformMesh->ComponentVelocity =(mc_UPlatformMesh->GetComponentLocation()-m_vLastLoc) *(mc_USpline->GetSplineLength() / (m_fTimeLineLength*10.f));
	//m_vLastLoc = mc_UPlatformMesh->GetComponentLocation();
	//FString msg = mc_UPlatformMesh->ComponentVelocity.ToString();
	//DrawDebugString(GetWorld(), GetActorLocation(), msg, this,FColor::Red,0.1f,true);
}

void AMovilePlatform::OnTimelineFinished()
{
	if (m_UIdleAudioComponent)
		m_UIdleAudioComponent->Stop();
	m_USndManager->PlayEventAtLocation(GetWorld(), m_UStopFMODEvent, mc_UPlatformMesh->GetComponentLocation(), true);
	
	m_vLastLoc = mc_UPlatformMesh->GetComponentLocation();
	mc_UPlatformMesh->ComponentVelocity = FVector::ZeroVector;
	if (!m_bNeedsPlayer)
	{
		if (m_bStartAgainSpline)
		{
			if (m_TPausesBetweenPoints.Num() < 2)
			{
				GetWorldTimerManager().SetTimer(m_DelayTimer, this, &AMovilePlatform::DelaySplineFalse, m_TPausesBetweenPoints[0], false);
			}
			else {
				GetWorldTimerManager().SetTimer(m_DelayTimer, this, &AMovilePlatform::DelaySplineFalse, m_TPausesBetweenPoints[1], false);
			}
		}
		else {
			if (m_TPausesBetweenPoints.Num() < 2)
			{
				GetWorldTimerManager().SetTimer(m_DelayTimer, this, &AMovilePlatform::DelaySplineTrue, m_TPausesBetweenPoints[0], false);
			}
			else {
				GetWorldTimerManager().SetTimer(m_DelayTimer, this, &AMovilePlatform::DelaySplineTrue, m_TPausesBetweenPoints[0], false);
			}
		}
	}
	else {
		m_bStartAgainSpline = !m_bStartAgainSpline;
	}
}

void AMovilePlatform::DelaySplineFalse()
{
	if (m_UIdleAudioComponent)
		m_UIdleAudioComponent->Play();
	m_bStartAgainSpline = false;
	m_FTimeline.Play();
}
void AMovilePlatform::DelaySplineTrue()
{
	if (m_UIdleAudioComponent)
		m_UIdleAudioComponent->Play();
	m_bStartAgainSpline = true;
	m_FTimeline.Reverse();
}
// Called when the game starts or when spawned
void AMovilePlatform::BeginPlay()
{
	Super::BeginPlay();
	m_vLastLoc = mc_UPlatformMesh->GetComponentLocation();
	m_FTimeline.SetPlayRate(1.f / m_fTimeLineLength);
	m_bActivatesKillingEnemies = mc_UDeathDetector->m_TObservedEnemies.Num() > 0;
	m_bPlatformUnlocked = !m_bActivatesKillingEnemies;
	if (m_UCurve)
	{
		FOnTimelineFloat Delegate;
		FOnTimelineEvent EndDelegate;
		EndDelegate.BindUFunction(this, FName("OnTimelineFinished"));
		Delegate.BindUFunction(this, FName("SplineTLFunc"));
		m_FTimeline.AddInterpFloat(m_UCurve,Delegate);
		m_FTimeline.SetTimelineFinishedFunc(EndDelegate);
	}
	if (m_bUseWaypoints)
	{
		//Waypoint mode, NOT IN USE. Code is here in case design wants it back
		if (m_TPausesBetweenPoints.Num()!=0 && m_TWaypoints.Num()!=0 && m_TPlatformSpeeds.Num()!=0)
		{
			m_bSameArraySizes = (m_TPausesBetweenPoints.Num() == m_TWaypoints.Num()) && (m_TWaypoints.Num() == m_TPlatformSpeeds.Num());
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::MovePlatform, 0.01f, true);
			m_FDstLoc = m_TWaypoints[m_iIndex]->GetActorLocation();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Please provide waypoints and at least one speed and pause for the platform"));
			m_bProvidedAllData = false;
		}
	}
	else if(!m_bUseWaypoints && m_UCurve)
	{
		if (m_TPausesBetweenPoints.Num() != 0) {
			m_USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
			m_UIdleAudioComponent = m_USndManager->PlayEventAttached(m_UIdleFMODEvent, mc_UPlatformMesh, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, false, false);
			if(!m_bActivatesKillingEnemies)
			{
				if (!m_bNeedsPlayer)
				{
					if (m_UIdleAudioComponent)
						m_UIdleAudioComponent->Play();
					m_FTimeline.Play();
				}
				
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("ERROR IN MOVILE PLATFORM: You must provide at least one pause time in the array"));

		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR IN MOVILE PLATFORM: Could not initialize platform with spline, curve wasn't found"));

	}

	
}

// Called every frame
void AMovilePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_FTimeline.TickTimeline(DeltaTime);
}

void AMovilePlatform::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	m_FTimeline.SetPlayRate(1.f / m_fTimeLineLength);
}

void AMovilePlatform::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (m_bNeedsPlayer)
	{
		if (!m_bActivatesKillingEnemies)
		{
			if (Character)
			{
				if (!m_bPlayerDetected)
				{
					if (m_bStartAgainSpline)
					{

						if (m_UIdleAudioComponent)
							m_UIdleAudioComponent->Play();
						m_FTimeline.Reverse();
					}
					else
					{
						if (m_UIdleAudioComponent)
							m_UIdleAudioComponent->Play();
						m_FTimeline.Play();
					}
				}
				m_bPlayerDetected = true;
				Character->AttachToComponent(mc_URoot, { EAttachmentRule::KeepWorld,EAttachmentRule::KeepWorld ,EAttachmentRule::KeepWorld ,false });
			}
			if (Cast<AMovilePlatform>(OtherActor))
			{
				m_bTransitionBetweenPlatforms = true;
			}
		}
		else {
			if (m_bPlatformUnlocked)
			{
				if (Character)
				{
					if (!m_bPlayerDetected)
					{
						if (m_UIdleAudioComponent)
							m_UIdleAudioComponent->Play();
						m_FTimeline.Play();
					}
					m_bPlayerDetected = true;
					Character->AttachToComponent(mc_URoot, { EAttachmentRule::KeepWorld,EAttachmentRule::KeepWorld ,EAttachmentRule::KeepWorld ,false });
				}
				if (Cast<AMovilePlatform>(OtherActor))
				{
					m_bTransitionBetweenPlatforms = true;
				}
			}
		}
	}
	else {
		if (Character)
		{
			Character->AttachToComponent(mc_URoot, { EAttachmentRule::KeepWorld,EAttachmentRule::KeepWorld ,EAttachmentRule::KeepWorld ,false });
		}
		if (Cast<AMovilePlatform>(OtherActor))
		{
			m_bTransitionBetweenPlatforms = true;
		}
	}
}

void AMovilePlatform::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (m_bNeedsPlayer)
	{
		if (!m_bActivatesKillingEnemies)
		{
			if (m_bTransitionBetweenPlatforms)
			{
				m_bTransitionBetweenPlatforms = false;
				//if (m_bPlayerDetected)
				//{
				//	if (m_bStartAgainSpline)
				//	{
				//		m_bPlayerDetected = false;
				//		m_bStartAgainSpline = false;
				//		DelaySplineFalse();
				//	}
				//	else {
				//		m_bPlayerDetected = false;
				//		m_bStartAgainSpline = true;
				//		DelaySplineTrue();
				//	}
				//}
			}
			else {
				if (Character)
				{
					if (m_bPlayerDetected)
					{
						if (m_bStartAgainSpline)
						{
							m_bPlayerDetected = false;
							m_bStartAgainSpline = false;
							DelaySplineFalse();
						}
						else {
							m_bPlayerDetected = false;
							m_bStartAgainSpline = true;
							DelaySplineTrue();
						}
					}
					Character->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				}
			}
		}
		else
		{
			if (m_bPlatformUnlocked)
			{
				if (m_bTransitionBetweenPlatforms)
				{
					m_bTransitionBetweenPlatforms = false;
					if (m_bPlayerDetected)
					{
						if (m_bStartAgainSpline)
						{
							m_bPlayerDetected = false;
							m_bStartAgainSpline = false;
							DelaySplineFalse();
						}
						else {
							m_bPlayerDetected = false;
							m_bStartAgainSpline = true;
							DelaySplineTrue();
						}
					}
				}
				else {
					if (Character)
					{
						if (m_bPlayerDetected)
						{
							if (m_bStartAgainSpline)
							{
								m_bPlayerDetected = false;
								m_bStartAgainSpline = false;
								DelaySplineFalse();
							}
							else {
								m_bPlayerDetected = false;
								m_bStartAgainSpline = true;
								DelaySplineTrue();
							}
						}
						Character->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
					}
				}
			}
		}
	}
	else {
		if (m_bTransitionBetweenPlatforms)
		{
			m_bTransitionBetweenPlatforms = false;
		}
		else {
			if (Character)
			{
				Character->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}

void AMovilePlatform::StopPlatform()
{
	//if (m_bNeedsPlayer)
	//{
	//	m_UIdleAudioComponent->Stop();
	//	m_FTimeline.Stop();
	//}
	//else {
	//	if (m_bActivatesKillingEnemies)
	//	{
	//		if (m_bPlatformUnlocked)
	//		{
	//			m_UIdleAudioComponent->Stop();
	//			m_FTimeline.Stop();
	//		}
	//	}
	//	else {
	//		m_UIdleAudioComponent->Stop();
	//		m_FTimeline.Stop();
	//	}
	//}
	if(m_UIdleAudioComponent)
		m_UIdleAudioComponent->Stop();
	m_FTimeline.Stop();
	SetActorTickEnabled(false);
}

void AMovilePlatform::ResumePlatform()
{
	SetActorTickEnabled(true);
	if (m_bNeedsPlayer)
	{
		if (m_bPlatformUnlocked)
		{
			if (m_bStartAgainSpline)
			{
				if (m_UIdleAudioComponent)
					m_UIdleAudioComponent->Play();
				m_FTimeline.Reverse();
			}
			else {
				if (m_UIdleAudioComponent)
					m_UIdleAudioComponent->Play();
				m_FTimeline.Play();
			}
		}
	}
	else {
		if (m_bActivatesKillingEnemies)
		{
			if (m_bPlatformUnlocked)
			{
				if (m_bStartAgainSpline)
				{
					if (m_UIdleAudioComponent)
						m_UIdleAudioComponent->Play();
					m_FTimeline.Reverse();
				}
				else {
					if (m_UIdleAudioComponent)
						m_UIdleAudioComponent->Play();
					m_FTimeline.Play();
				}
			}
		}
		else {
			if (m_bStartAgainSpline)
			{
				if (m_UIdleAudioComponent)
					m_UIdleAudioComponent->Play();
				m_FTimeline.Reverse();
			}
			else {
				if (m_UIdleAudioComponent)
					m_UIdleAudioComponent->Play();
				m_FTimeline.Play();
			}
		}
	}
	//if ()
	//{
	//	if (m_bStartAgainSpline)
	//	{
	//		m_UIdleAudioComponent->Play();
	//		m_FTimeline.Reverse();
	//	}
	//	else {
	//		m_UIdleAudioComponent->Play();
	//		m_FTimeline.Play();
	//	}
	//}
}

void AMovilePlatform::MovePlatform()
{
	if (m_bSameArraySizes)
	{
		FVector Dir = m_FDstLoc - mc_UPlatformMesh->GetComponentLocation();
		float Dist = Dir.Size();
		UKismetMathLibrary::Vector_Normalize(Dir);
		mc_UPlatformMesh->AddLocalOffset(Dir*m_TPlatformSpeeds[m_iIndex]*UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));
		if (Dist <= m_fReachDistance)
		{
			GetWorldTimerManager().ClearTimer(m_FPlatformTimer);
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::Delay1, m_TPausesBetweenPoints[m_iIndex], false);
		}
	}
	else {
		FVector Dir = m_FDstLoc - mc_UPlatformMesh->GetComponentLocation();
		float Dist = Dir.Size();
		UKismetMathLibrary::Vector_Normalize(Dir);
		mc_UPlatformMesh->AddLocalOffset(Dir * m_TPlatformSpeeds[0] * UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));
		if (Dist <= m_fReachDistance)
		{
			GetWorldTimerManager().ClearTimer(m_FPlatformTimer);
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::Delay2, m_TPausesBetweenPoints[0], false);
		}
	}
}

void AMovilePlatform::ReturnPlatform()
{
	if (m_bSameArraySizes)
	{
		FVector Dir = m_FDstLoc - mc_UPlatformMesh->GetComponentLocation();
		float Dist = Dir.Size();
		UKismetMathLibrary::Vector_Normalize(Dir);
		mc_UPlatformMesh->AddLocalOffset(Dir * m_TPlatformSpeeds[m_iIndex] * UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));
		if (Dist <= m_fReachDistance)
		{
			GetWorldTimerManager().ClearTimer(m_FPlatformTimer);
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::Delay3, m_TPausesBetweenPoints[m_iIndex], false);
		}
	}
	else {
		FVector Dir = m_FDstLoc - mc_UPlatformMesh->GetComponentLocation();
		float Dist = Dir.Size();
		UKismetMathLibrary::Vector_Normalize(Dir);
		mc_UPlatformMesh->AddLocalOffset(Dir * m_TPlatformSpeeds[0] * UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));
		if (Dist <= m_fReachDistance)
		{
			GetWorldTimerManager().ClearTimer(m_FPlatformTimer);
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::Delay4, m_TPausesBetweenPoints[0], false);
		}
	}
}

void AMovilePlatform::Delay1()
{
	m_iIndex++;
	if (m_iIndex == m_TWaypoints.Num()) {
		if (m_bIsCircular)
		{
			m_iIndex = 0;
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::MovePlatform, 0.01f, true);
		}
		else {
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::ReturnPlatform, 0.01f, true);

		}
	}
	else {
		GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::MovePlatform, 0.01f, true);
		m_iIndex = UKismetMathLibrary::Clamp(m_iIndex - 2, 0, m_TWaypoints.Num());
		m_FDstLoc = m_TWaypoints[m_iIndex]->GetActorLocation();
	}
}

void AMovilePlatform::Delay2()
{
	m_iIndex++;
	if (m_iIndex == m_TWaypoints.Num()) {
		if (m_bIsCircular)
		{
			m_iIndex = 0;
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::MovePlatform, 0.01f, true);
		}
		else {
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::ReturnPlatform, 0.01f, true);

		}
	}
	else {
		GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::MovePlatform, 0.01f, true);
		m_iIndex = UKismetMathLibrary::Clamp(m_iIndex - 2, 0, m_TWaypoints.Num());
		m_FDstLoc = m_TWaypoints[m_iIndex]->GetActorLocation();
	}
}

void AMovilePlatform::Delay3()
{
	m_iIndex--;
	if (m_iIndex == -1) {
		if (m_bIsCircular)
		{
			m_iIndex = 1;
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::MovePlatform, 0.01f, true);
		}
		else {
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::ReturnPlatform, 0.01f, true);

		}
	}
	else {
		GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::MovePlatform, 0.01f, true);
		m_iIndex = UKismetMathLibrary::Clamp(m_iIndex - 2, 0, m_TWaypoints.Num());
		m_FDstLoc = m_TWaypoints[m_iIndex]->GetActorLocation();
	}
}

void AMovilePlatform::Delay4()
{
	m_iIndex--;
	if (m_iIndex == -1) {
		if (m_bIsCircular)
		{
			m_iIndex = 1;
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::MovePlatform, 0.01f, true);
		}
		else {
			GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::ReturnPlatform, 0.01f, true);

		}
	}
	else {
		GetWorldTimerManager().SetTimer(m_FPlatformTimer, this, &AMovilePlatform::MovePlatform, 0.01f, true);
		m_iIndex = UKismetMathLibrary::Clamp(m_iIndex - 2, 0, m_TWaypoints.Num());
		m_FDstLoc = m_TWaypoints[m_iIndex]->GetActorLocation();
	}
}

void AMovilePlatform::OnAllEnemiesDeathPlatform()
{
	if (m_bNeedsPlayer)
	{
		m_bPlatformUnlocked = true;
	}
	else {
		m_bPlatformUnlocked = true;
		if (m_UIdleAudioComponent)
			m_UIdleAudioComponent->Play();
		m_FTimeline.Play();
	}
}

