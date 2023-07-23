// Fill out your copyright notice in the Description page of Project Settings.


#include "BorderHighlight.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"

// Sets default values for this component's properties
UBorderHighlight::UBorderHighlight()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

bool UBorderHighlight::CheckIfShouldActivate()
{
	const bool bDistanceTest = FVector::DistSquared(m_APlayerPtr->GetActorLocation(), GetOwner()->GetActorLocation()) < m_fRangeToShowBorder * m_fRangeToShowBorder;
	const bool bAngleTest = UKismetMathLibrary::Dot_VectorVector(
		m_APlayerPtr->m_vBatDirection,
		(GetOwner()->GetActorLocation() - m_APlayerPtr->GetActorLocation()).GetSafeNormal()
	) >= m_fAngleCos;
	
	return bDistanceTest && bAngleTest;
}

#if WITH_EDITOR
void UBorderHighlight::PostEditChangeProperty(FPropertyChangedEvent& _FPropertyChangedEvent)
{
	Super::PostEditChangeProperty(_FPropertyChangedEvent);

	if(GET_MEMBER_NAME_CHECKED(UBorderHighlight, m_FMeshComponentName) == _FPropertyChangedEvent.GetPropertyName())
	{
		m_TMeshesToHighlight.Empty();
		UMeshComponent* UFoundMeshComponent = Cast<UMeshComponent>(GetOwner()->GetDefaultSubobjectByName(m_FMeshComponentName));
		if(IsValid(UFoundMeshComponent))
		{
			m_TMeshesToHighlight.Add(UFoundMeshComponent);
		}
	}
}
#endif

// Called when the game starts
void UBorderHighlight::BeginPlay()
{
	Super::BeginPlay();

	m_APlayerPtr = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));

	m_fAngleCos = UKismetMathLibrary::DegCos(m_fAngleToShowBorder/2.0f);

	if(!m_bSelectMeshByName)
	{
		GetOwner()->GetComponents(m_TMeshesToHighlight);
	}
	else if(m_TMeshesToHighlight.Num() < 1)
	{
		m_TMeshesToHighlight.Empty();
		UMeshComponent* UFoundMeshComponent = Cast<UMeshComponent>(GetOwner()->GetDefaultSubobjectByName(m_FMeshComponentName));
		if(IsValid(UFoundMeshComponent))
		{
			m_TMeshesToHighlight.Add(UFoundMeshComponent);
		}
	}

	if(m_TMeshesToHighlight.Num() > 0)
	{
		for(UMeshComponent* UMeshToHighlight : m_TMeshesToHighlight)
		{
			UMeshToHighlight->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_Default);
			UMeshToHighlight->SetCustomDepthStencilValue(1);
		}
	}
	else
	{
		this->SetComponentTickEnabled(false);
	}
}


// Called every frame
void UBorderHighlight::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (m_bBorderEnabled)
	{
		if (CheckIfShouldActivate() && !m_bIsActive)
		{
			m_bIsActive = true;
			for (UMeshComponent* UMeshToHighlight : m_TMeshesToHighlight)
			{
				UMeshToHighlight->SetRenderCustomDepth(m_bIsActive);
			}
		}
		else if (!CheckIfShouldActivate() && m_bIsActive)
		{
			m_bIsActive = false;
			for (UMeshComponent* UMeshToHighlight : m_TMeshesToHighlight)
			{
				UMeshToHighlight->SetRenderCustomDepth(m_bIsActive);
			}
		}
	}
}

void UBorderHighlight::EnableBorderHighLight()
{
	m_bBorderEnabled = true;
}

void UBorderHighlight::DisableBorderHighLight()
{
	m_bBorderEnabled = false;
	m_bIsActive = false;
	for (UMeshComponent* UMeshToHighlight : m_TMeshesToHighlight)
	{
		UMeshToHighlight->SetRenderCustomDepth(m_bIsActive);
	}
}