// Fill out your copyright notice in the Description page of Project Settings.


#include "Barrier.h"

#include "Components/TextRenderComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"

// Sets default values
ABarrier::ABarrier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mc_UPivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	SetRootComponent(mc_UPivot);

	mc_UWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall"));
	mc_UWall->SetupAttachment(mc_UPivot);

	mc_UCounter = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Counter"));
	mc_UCounter->SetupAttachment(mc_UPivot);
}

void ABarrier::OnEnemyDeath()
{
	--m_iCounter;

	if(m_iCounter <= 0)
	{
		m_iCounter = 0;
		Open();
	}
}

// Called when the game starts or when spawned
void ABarrier::BeginPlay()
{
	Super::BeginPlay();
	
	for(const ABaseEnemy* AEnemy : TEnemies)
	{
		if(IsValid(AEnemy))
		{
			++m_iCounter;
			AEnemy->mc_pHealthComp->OnDeath.AddDynamic(this, &ABarrier::OnEnemyDeath);
		}
	}

	m_VStartPos = GetActorLocation() + m_VLocalStartPos;
	m_VEndPos = GetActorLocation() + m_VLocalEndPos;
	
	mc_UCounter->SetText(FText::FromString(FString::FromInt(m_iCounter)));
}

// Called every frame
void ABarrier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if WITH_EDITOR
	if(GIsPlayInEditorWorld)
	{
#endif
		if(m_bIsOpening)
		{
			m_fMovementProgress += DeltaTime * m_fCloseSpeed;

			const FVector VNextLocation = FMath::Lerp(m_VStartPos, m_VEndPos, m_fMovementProgress);
			SetActorLocation(VNextLocation);

			if(m_fMovementProgress >= 1)
			{
				m_fMovementProgress = 1.0f;
				m_bIsOpening = false;
			}
		}
		else if(m_bIsClosing)
		{
			m_fMovementProgress -= DeltaTime * m_fCloseSpeed;

			const FVector VNextLocation = FMath::Lerp(m_VStartPos, m_VEndPos, m_fMovementProgress);
			SetActorLocation(VNextLocation);

			if(m_fMovementProgress <= 0)
			{
				m_fMovementProgress = 0.0f;
				m_bIsClosing = false;
			}
		}
#if WITH_EDITOR
	}
	else
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), GetActorLocation() + m_VLocalStartPos, 20, 24, FLinearColor::Green, DeltaTime*2, 3.0f);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), GetActorLocation() + m_VLocalEndPos, 20, 24, FLinearColor::Blue, DeltaTime*2, 3.0f);
	}
#endif
}

#if WITH_EDITOR
bool ABarrier::ShouldTickIfViewportsOnly() const
{
	return true;
}

void ABarrier::EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown,
	bool bCtrlDown)
{
	Super::EditorApplyScale(DeltaScale, PivotLocation, bAltDown, bShiftDown, bCtrlDown);
	
	mc_UWall->SetRelativeScale3D(mc_UWall->GetRelativeScale3D() + DeltaScale);
	mc_UWall->SetRelativeLocation(FVector(0.0f,0.0f,mc_UWall->GetRelativeScale3D().Z*50));
	
	const FVector VWallScale = mc_UWall->GetRelativeScale3D();
	const FVector VNewLocation = FVector(-50.0f * VWallScale.X + 5.0f,50.0f * VWallScale.Y + 1,100.0f * VWallScale.Z - 70.f);
	mc_UCounter->SetRelativeLocation(VNewLocation);
	
	SetActorScale3D(FVector(1.0f,1.0f,1.0f));
}
#endif

void ABarrier::Open()
{
	m_bIsOpening = true;
	m_bIsClosing = false;
}

void ABarrier::Close()
{
	m_bIsOpening = false;
	m_bIsClosing = true;
}

