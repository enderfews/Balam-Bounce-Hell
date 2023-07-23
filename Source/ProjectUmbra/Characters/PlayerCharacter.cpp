// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "ProjectUmbra/Items/AProjectile.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "ProjectUmbra/Items/TagDrop.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "ProjectUmbra/System/ProbabilityManager.h"
#include "ProjectUmbra/Widgets/PlayerHUD.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "UObject/ConstructorHelpers.h"
#include "ProjectUmbra/Widgets/ToggleController.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraCommon.h"
#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectUmbra/System/SoundManager.h"
#include "ProjectUmbra/System/UmbraGameMode.h"
#include "Automatinator/DeathRecorderComponent.h"
#include "Automatinator/PathRecorderComponent.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"


// Sets default values
//TODO: Debug UI
APlayerCharacter::APlayerCharacter() : m_fLookAtControllerV(0.f),m_fLookAtControllerH(0.f),m_fMoveToControllerV(0.f),m_fMoveToControllerH(0.f),m_fKnockbackDuration(0.f)
,m_fStunDuration(0.f),m_fChangeSpeed(10.f),m_vDebugForward(FVector(0.f,0.f,0.f)),m_vDebugRight(FVector(0.f,0.f,0.f)),m_FDebugColor(FLinearColor(0.066667f, 0.854902f, 1.f, 1.f)),m_UBatAreaMaterialDynamic(nullptr),m_fBatAngleCos(0.f),
m_fTimeToResetDuration(0.f),m_fTimeToResetCooldown(0.f),m_bIsBatInCooldown(false),m_bIsBatRunning(false),m_bIsDashing(false),m_vVelocity(FVector(0.f, 0.f, 0.f)),m_bIdle(false),m_fBrakingFrictionFactor(0.f),m_fBrakingDecelerationWalking(0.f),m_fGravityScale(0.f),
m_bCanDash(true),m_fTimeToResetDashDuration(0.f),m_fTimeToResetDashCooldown(0.f),m_fTimeToResetDashImmunityTime(0.f),m_vLastJoystickPos(FVector(0.f,0.f,0.f)),m_fTimeToResetAiming(0.f),m_bIsStunned(false),m_bIsDebugChanged(false), m_bIsControllerWidgetVisible(false),m_fMovementSpeed(500.f),m_bIsDebugOn(false), m_bActivateRecordData(false),m_fMaxLengthChange(0.f),m_fMinLengthChange(0.f),m_fAngleChange(0.f),
m_fConeAlphaActive(0.6f),m_fConeAlphaInactive(0.01f),m_fBatCooldown(0.4f),m_fBatDuration(0.3f),m_fBatMaxLength(350.f),m_fBatMinLength(100.f),m_fBatAngle(60.f),m_fAutoaimRange(2000.f),m_fAutoaimAngle(30.f),m_iMaxTagAmmo(10),m_fDashSpeed(2700.f),m_fDashZVelocity(300.f),
m_fDashRate(3.f),m_fDashDuration(0.1f),m_fImmunityTime(0.5f),m_bIsUsingController(false),m_fDashDistanceVariation(50.f),m_bDashStart(false),m_fCoyoteTimeDuration(0.05f),m_fTimeToResetCoyoteTime(0.f),m_bCoyoteTimeActive(false),
m_fLeftCenterDeadzone(0.4f),m_fRightCenterDeadzone(0.4f),m_fLastJoystickMem(2.f),m_ACamera(nullptr),m_bInGodMode(false),m_bInTestMode(false),
m_eTag(EBulletModifier::None),m_fCutCooldown(1.f),m_vMovementDirection(FVector(1.f,0.f,0.f)),m_vBatDirection(FVector(0.f,0.f,0.f)),m_bAlphaWhenNotAiming(true),m_bIsAiming(false),m_bIsGrounded(true),m_fGroundedCheckHeight(100.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	mc_WControls = CreateDefaultSubobject<UWidgetComponent>(TEXT("ControlsWidget"));
	mc_WControls->SetupAttachment(RootComponent);
	mc_WControllerToggle = CreateDefaultSubobject<UWidgetComponent>(TEXT("ControllerToggleWidget"));
	mc_WControllerToggle->SetupAttachment(RootComponent);
	mc_WHealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	mc_WHealthBar->SetupAttachment(RootComponent);
	mc_UInteractPopUp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractPopUpWidget"));
	mc_UInteractPopUp->SetupAttachment(RootComponent);
	mc_UAimConeDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("AimDecal"));
	mc_UAimConeDecal->SetupAttachment(RootComponent);
	mc_UAimMeshPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AimMeshPlane"));
	mc_UAimMeshPlane->SetupAttachment(RootComponent);
	mc_UMouseAimPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MauseAimMesh"));
	mc_UMouseAimPlane->SetupAttachment(RootComponent);
	mc_UMovArrowPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MovArrowMesh"));
	mc_UMovArrowPlane->SetupAttachment(RootComponent);

	mc_UHealth = CreateDefaultSubobject<UHealthComponentCpp>(TEXT("HealthComponent"));
	mc_UStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));

	mc_WControllerToggle->SetVisibility(m_bIsControllerWidgetVisible);
	
	m_fBatAngleCos = UKismetMathLibrary::DegCos(m_fBatAngle / 2.0f);


	mc_UDeathRecorder = CreateDefaultSubobject<UDeathRecorderComponent>(TEXT("Death recorder"));
	mc_UPathRecorder = CreateDefaultSubobject<UPathRecorderComponent>(TEXT("Path recorder"));
	mc_UActionRecorder = CreateDefaultSubobject<UActionRecorderComponent>(TEXT("Action Recorder"));
	OnTakeAnyDamage.AddDynamic(this, &APlayerCharacter::OnAnyDamage);
	MovementModeChangedDelegate.AddDynamic(this, &APlayerCharacter::OnStartFalling);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	m_UPlayerHUD = CreateWidget<UPlayerHUD>(GetWorld(), m_TPlayerHUDClass);
	m_UPlayerHUD->SetMaxAmmo(m_iMaxTagAmmo);
	m_UPlayerHUD->SetMaxHealth(mc_UHealth->m_fMaxHealth);
	m_UPlayerHUD->SetMaxComboLvl(m_iMaxLvlCombo);
	m_UPlayerHUD->SetPercentForCut(m_fCutComboCost / m_fComboMax);
	m_UPlayerHUD->SetComboMaxTime(m_fComboDuration);
	m_UPlayerHUD->SetCurrentComboPercent(0.0f);
	
	UProjectUmbraGameInstance* pGI = Cast<UProjectUmbraGameInstance>(GetGameInstance());
	if (pGI && !pGI->m_bIsMainMenu)
	{
		m_UPlayerHUD->AddToViewport();
	}

	mc_UHealth->OnDamaged.AddDynamic(this, &APlayerCharacter::OnDamagedCallback);
	mc_UHealth->OnDeath.AddDynamic(this, &APlayerCharacter::OnDeathCallback);
	mc_UHealth->OnHealed.AddDynamic(this, &APlayerCharacter::OnHealCallback);
	mc_UHealth->OnDamagedEnd.AddDynamic(this, &APlayerCharacter::OnEndDamagedCallback);
	mc_UHealth->OnImmunityEnd.AddDynamic(this, &APlayerCharacter::OnEndImmunityCallback);
	mc_UHealth->OnHealthChanged.AddDynamic(this, &APlayerCharacter::OnHealthChangedCallback);
	AUmbraGameMode* pGameMode = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	mc_UDeathRecorder->m_bActivateRecorder = m_bActivateRecordData;
	mc_UPathRecorder->m_bActivateRecorder = m_bActivateRecordData;
	mc_UActionRecorder->m_bActivateRecorder = m_bActivateRecordData;
	if (pGameMode && mc_UDeathRecorder->m_bActivateRecorder)
	{
		pGameMode->OnPlayerKilled.AddDynamic(mc_UDeathRecorder, &UDeathRecorderComponent::RecordOnDeath);
	}
	m_OnPlayerAction.AddDynamic(mc_UActionRecorder, &UActionRecorderComponent::Record);
	mc_UMovArrowPlane->SetWorldScale3D(FVector(2.0f,2.0f,2.0f));
	mc_UMovArrowPlane->SetRelativeLocation(FVector(0.0f,0.0f,-88.0f));
	mc_UMovArrowPlane->SetRelativeRotation(FRotator(0.0f,90.0f,0.0f));
	
	//TODO: Get Camera when C++ code is done

	if (Controller && Controller->IsPlayerController())
		Cast<APlayerController>(Controller)->SetShowMouseCursor(true);


	mc_UInteractPopUp->SetHiddenInGame(true);
	
	SetUpBatAreaEffect();

	for(int32 i = 0; i < GetMesh()->GetMaterials().Num(); ++i)
	{
		m_UMeshMaterials.Add(GetMesh()->CreateDynamicMaterialInstance(i, GetMesh()->GetMaterial(i)));
	}
	
	UProjectUmbraGameInstance* UGameInstance = GetGameInstance<UProjectUmbraGameInstance>();

	//Load data from previous map
	UGameInstance->GetProbabilityManager()->SetPlayer(this);

	const float fStoredHealth = UGameInstance->GetStoredPlayerHealth();
	const float fStoredCombo = UGameInstance->GetStoredPlayerCombo();

	if(fStoredHealth > 0.0f)
	{
		mc_UHealth->SetHealth(fStoredHealth);
		m_UPlayerHUD->SetCurrentHealth(fStoredHealth);
		UGameInstance->SetStoredPlayerHealth(-1.0f);
	}

	if(fStoredCombo > 0.0f)
	{
		m_fCombo = fStoredCombo;
		m_UPlayerHUD->m_bFirstComboBarFilled = false;
		m_UPlayerHUD->m_bSecondComboBarFilled = false;
		m_UPlayerHUD->m_bThirdComboBarFilled = false;
		m_UPlayerHUD->SetCurrentComboPercent(fStoredCombo/m_fComboMax);
		UGameInstance->SetStoredPlayerCombo(-1.0f);
	}

	m_fCurrentMovementSpeed = m_fMovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = m_fCurrentMovementSpeed;

	m_fCurrentCutCost = m_fCutComboCost;
	m_fCurrentDashCooldown = m_fDashRate;
	m_fCurrentBatCooldown = m_fBatCooldown;

	m_fGravityScale = GetCharacterMovement()->GravityScale;
	
	mc_UDashNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(m_UDashParticleSystem,RootComponent,
	TEXT("Dash Niagara"),FVector::ZeroVector,FRotator::ZeroRotator,
	EAttachLocation::KeepRelativeOffset,false,false,
			ENCPoolMethod::ManualRelease, true);

	mc_UHealedNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(m_UHealedParticleSystem,RootComponent,
	TEXT("Healed Niagara"),FVector::ZeroVector,FRotator::ZeroRotator,
	EAttachLocation::KeepRelativeOffset,false,false,
			ENCPoolMethod::ManualRelease, true);
	
	mc_UElectricTagNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(m_UElectricTagParticleSystem,RootComponent,
	TEXT("Electrict Tag Niagara"),FVector::ZeroVector,FRotator::ZeroRotator,
	EAttachLocation::KeepRelativeOffset,false,false,
			ENCPoolMethod::ManualRelease, true);

	m_USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
	m_USpecialAudioComponent = m_USndManager->PlayEventAttached(m_USpecialFMODEvent, GetRootComponent(), FVector::ZeroVector, EAttachLocation::KeepRelativeOffset, true, false, false);
}

void APlayerCharacter::UpdateTimers(float _fDeltaTime)
{
	// Update CUT cooldown timer
	if (m_fTimeToResetCutCooldown > 0.f) m_fTimeToResetCutCooldown -= _fDeltaTime;
	m_bIsCutInCooldown = m_fTimeToResetCutCooldown > 0.f;

	// Update CUT duration timer
	if (m_fTimeToResetCutDuration > 0.f)
	{
		m_fTimeToResetCutDuration -= _fDeltaTime;
	} else
	{
		m_TToIgnoreDuringCut.Empty();
		m_fCutExpansionProgress = 0.0f;
	}
	m_bIsCutRunning = m_fTimeToResetCutDuration > 0.f;
	
	// Update BAT cooldown timer
	if (m_fTimeToResetCooldown > 0.f) m_fTimeToResetCooldown -= _fDeltaTime;
	m_bIsBatInCooldown = m_fTimeToResetCooldown > 0.f;
	
	// Update BAT duration timer
	if (m_fTimeToResetDuration > 0.f) m_fTimeToResetDuration -= _fDeltaTime;
	m_bIsBatRunning = m_fTimeToResetDuration > 0.f;

	
	// Update reset joystick/aiming timer
	if (m_fTimeToResetAiming > 0.f) m_fTimeToResetAiming -= _fDeltaTime;
	m_bUseMovementAim = m_fTimeToResetAiming <= 0.f;


	// Start dash
	if (m_bDashStart)
	{
		m_bDashStart = false;
		GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
		GetCharacterMovement()->MaxWalkSpeed = m_fDashSpeed;
		GetCharacterMovement()->MaxAcceleration += 100000;
		GetCharacterMovement()->Velocity.Set(m_vMovementDirection.X * m_fDashSpeed, m_vMovementDirection.Y * m_fDashSpeed, m_fDashZVelocity);
		m_vVelocity = GetCharacterMovement()->Velocity;
		LaunchCharacter(m_vVelocity, true, true);
	}
	// Update DASH duration timer
	if (m_fTimeToResetDashDuration > 0.f)
	{
		m_fTimeToResetDashDuration -= _fDeltaTime;

		// Correct final position so the distance traveled by the dash will never be more than the max (duration * speed)
		if (m_fTimeToResetDashDuration < 0.f)
		{
			const TArray<AActor*> TActorsToIgnore;
			FHitResult FHitResult;
			const FVector vCurrentLocation = GetActorLocation();
			FVector vFinalLocation = vCurrentLocation + m_vMovementDirection * GetCharacterMovement()->MaxWalkSpeed * m_fTimeToResetDashDuration;

			if (UKismetSystemLibrary::LineTraceSingle(
				GetWorld(),
				vCurrentLocation,
				vFinalLocation,
				UEngineTypes::ConvertToTraceType(ECC_Visibility),
				false,
				TActorsToIgnore,
				EDrawDebugTrace::None,
				FHitResult,
				true
			)
				)
			{
				vFinalLocation = FHitResult.ImpactPoint - m_vMovementDirection * GetCapsuleComponent()->GetUnscaledCapsuleRadius();
			}
			SetActorLocation(vFinalLocation);
		}
	}
	// End dash
	else if (m_bIsDashing)
	{
		m_bIsDashing = false;
		mc_UDashNiagaraComponent->Deactivate();

		GetCharacterMovement()->Velocity.Set(0.f, 0.f, 0.f);
		GetCharacterMovement()->MaxWalkSpeed = m_fCurrentMovementSpeed;
		GetCharacterMovement()->MaxAcceleration -= 100000;
		GetCharacterMovement()->BrakingFrictionFactor = m_fBrakingFrictionFactor;
		GetCharacterMovement()->BrakingDecelerationWalking = m_fBrakingDecelerationWalking;
		GetCharacterMovement()->GravityScale = m_fGravityScale;
		
		EnableInput(Cast<APlayerController>(this));
		m_fTimeToResetDashImmunityTime = m_fImmunityTime;
		m_fTimeToResetDashCooldown = m_fCurrentDashCooldown;
	}

	// Update DASH immunity time timer
	if (m_fTimeToResetDashImmunityTime > 0.f) m_fTimeToResetDashImmunityTime -= _fDeltaTime;
	else mc_UHealth->m_bGodMode = m_bIsDashing || m_bInGodMode;

	// Update DASH cooldown timer
	if (m_fTimeToResetDashCooldown > 0.f) m_fTimeToResetDashCooldown -= _fDeltaTime;
	else if (m_bCanDash == false && m_bIsDashing == false && m_bIsGrounded == true) m_bCanDash = true;

	// Update Coyote Time timer
	if (m_fTimeToResetCoyoteTime > 0.f) m_fTimeToResetCoyoteTime -= _fDeltaTime;
	else if (m_bCoyoteTimeActive == true)
	{
		GetCharacterMovement()->GravityScale = m_fGravityScale;
		m_bCoyoteTimeActive = false;
	}

	//Update Knockback timers
	if(m_bIsStunned)
	{
		if(m_fKnockbackDuration > 0)
		{
			m_fKnockbackDuration -= _fDeltaTime;
			if(m_fKnockbackDuration < 0)
			{
				m_fStunDuration += m_fKnockbackDuration;
			}
		}
		else if(m_fStunDuration > 0)
		{
			m_fStunDuration -= _fDeltaTime;
			m_vMovementDirection = FVector(0,0,0);
		} else
		{
			m_bIsStunned = false;
			EnableInput(Cast<APlayerController>(this));
			GetCharacterMovement()->MaxWalkSpeed = m_fCurrentMovementSpeed;
		}
	}

	if(m_fSlowDuration > 0)
	{
		m_fSlowDuration -= _fDeltaTime;
	}
	else
	{
		m_fCurrentMovementSpeed = m_fMovementSpeed;
		if(!m_bIsDashing)
		{
			GetCharacterMovement()->MaxWalkSpeed = m_fCurrentMovementSpeed;
		}
	}

	// Update COMBO timer
	if (m_fTimeToResetCombo > 0.0f)
	{
		m_fTimeToResetCombo -= _fDeltaTime;
		m_UPlayerHUD->SetComboRemainingTime(m_fTimeToResetCombo);
	}
	else
	{
		SetComboLevel(0);
		m_UPlayerHUD->SetCurrentComboLvl(0);
		m_UPlayerHUD->SetComboRemainingTime(0.0f);
	}

	// Update ControllerToggleHUD timer
	if (m_fTimeToResetControllerHUD > 0.f) m_fTimeToResetControllerHUD -= _fDeltaTime;
	else ToggleControllerWidget();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateTimers(DeltaTime);
	if (!m_bInTestMode)
	{
		Move(DeltaTime);
		if (m_bIsBatRunning) Bat();
		Aim();
		DrawAimArea();
		if (m_bIsCutRunning) Cut();
		CheckGrounded();

	}

	//Billboard behaviour for widgets
	if(!mc_UInteractPopUp->bHiddenInGame)
	{
		mc_UInteractPopUp->SetWorldRotation((m_ACamera->GetActorLocation() - GetActorLocation()).Rotation());
	}
}

void APlayerCharacter::CheckGrounded()
{
	// TODO: Why is IgnoreSelf not working?
	TArray<AActor*> TActorsToIgnore;
	TActorsToIgnore.Add(this);
	FHitResult FHitResult;

	if(UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		GetActorLocation(),
		FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - m_fGroundedCheckHeight),
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TActorsToIgnore,
		EDrawDebugTrace::None,
		FHitResult,
		true))
	{
		m_bIsGrounded = true;
	}
	else
	{
		m_bIsGrounded = false;
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis("MoveToControllerVertical", this, &APlayerCharacter::MoveForwardAxis);
	PlayerInputComponent->BindAxis("MoveToControllerHorizontal", this, &APlayerCharacter::MoveRightAxis);
	PlayerInputComponent->BindAxis("LookAtControllerVertical", this, &APlayerCharacter::RotateForwardAxis);
	PlayerInputComponent->BindAxis("LookAtControllerHorizontal", this, &APlayerCharacter::RotateRightAxis);

	PlayerInputComponent->BindAction("Bat", IE_Pressed, this, &APlayerCharacter::Bat);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerCharacter::Dash);
	PlayerInputComponent->BindAction("Cut", IE_Pressed, this, &APlayerCharacter::Cut);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
	PlayerInputComponent->BindAction("ToggleController", IE_Pressed, this, &APlayerCharacter::ToggleController);
	
}

void APlayerCharacter::PlayStepSound()
{
	if(m_bIsGrounded && !m_bIsDashing)
		m_USndManager->PlayEventAtLocation(GetWorld(), m_UStepFMODEvent, GetActorLocation());
}

void APlayerCharacter::MoveForwardAxis(float Value)
{
	m_fMoveToControllerV = !m_bIsStunned ? Value : 0.f;	
}

void APlayerCharacter::MoveRightAxis(float Value)
{
	m_fMoveToControllerH = !m_bIsStunned ? Value : 0.f;
}

void APlayerCharacter::RotateForwardAxis(float Value)
{
	m_fLookAtControllerV = !m_bIsStunned ? Value : 0.f;
}

void APlayerCharacter::RotateRightAxis(float Value)
{
	m_fLookAtControllerH = !m_bIsStunned ? Value : 0.f;
}

void APlayerCharacter::ReceiveSlow(float _fSlowDuration, float _fSlowAmount)
{
	m_fCurrentMovementSpeed = m_fMovementSpeed * (1 - _fSlowAmount);
	m_fSlowDuration = _fSlowDuration;
	if(!m_bIsDashing)
	{
		GetCharacterMovement()->MaxWalkSpeed = m_fCurrentMovementSpeed;
	}
}

void APlayerCharacter::ReceiveKnockback(const FVector& _vDir, float _fTime, float _fSpeed, float _fStunTime)
{
	if(m_bIsDashing)
	{
		// Deactivate dash
		m_bIsDashing = false;
		mc_UDashNiagaraComponent->Deactivate();
		
		m_fTimeToResetDashCooldown = m_fCurrentDashCooldown;
		m_fTimeToResetDashImmunityTime = 0.0f;
		mc_UHealth->m_bGodMode = m_bIsDashing || m_bInGodMode;
		GetCharacterMovement()->Velocity.Set(0.f, 0.f, 0.f);
		GetCharacterMovement()->MaxAcceleration -= 100000;
		GetCharacterMovement()->BrakingFrictionFactor = m_fBrakingFrictionFactor;
		GetCharacterMovement()->BrakingDecelerationWalking = m_fBrakingDecelerationWalking;
		GetCharacterMovement()->GravityScale = m_fGravityScale;
	}
	m_bIsStunned = true;
	m_vMovementDirection = _vDir;
	GetCharacterMovement()->MaxWalkSpeed = _fSpeed;
	m_fKnockbackDuration = _fTime;
	m_fStunDuration = _fStunTime;
	DisableInput(UGameplayStatics::GetPlayerController(GetWorld(),0));
}

void APlayerCharacter::RemoveControls(float _fDuration)
{
	UE_LOG(LogTemp, Error, TEXT("Remove controls called, duration:%f"), _fDuration);
	m_fStunDuration = _fDuration;
	m_bIsStunned = true;
	m_vMovementDirection = FVector::ZeroVector;
	DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void APlayerCharacter::ActivateControls()
{
	EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void APlayerCharacter::Dash()
{
	if (!m_bIsStunned && m_bCanDash)
	{
		if (!GetCharacterMovement()->IsFalling() || m_bCoyoteTimeActive == true)
		{
			OnPlayerDash();
			
			PlayAnimMontage(m_UDashAnimMontage, 2.0f, NAME_None);
			mc_UDashNiagaraComponent->Activate(true);

			m_USndManager->PlayEventAtLocation(GetWorld(), m_UDashFMODEvent, GetActorLocation());

			m_bCanDash = false;
			m_bIsDashing = true;
			m_bDashStart = true;
			mc_UHealth->m_bGodMode = true;

			DisableInput(Cast<APlayerController>(this));
			GetCharacterMovement()->MovementMode = EMovementMode::MOVE_None;
			m_fBrakingFrictionFactor = GetCharacterMovement()->BrakingFrictionFactor;
			m_fBrakingDecelerationWalking = GetCharacterMovement()->BrakingDecelerationWalking;
			if (GetCharacterMovement()->GravityScale != 0.f)
			{
				GetCharacterMovement()->GravityScale = 0.f;
			}
			GetCharacterMovement()->BrakingFrictionFactor = 0.f;
			GetCharacterMovement()->BrakingDecelerationWalking = 0.f;
			m_fTimeToResetDashDuration = m_fDashDuration;

			FHitResult OutHit(ForceInit);
			m_vEndPosition = GetActorLocation() + m_vMovementDirection * m_fDashSpeed * m_fDashDuration;
			GetWorld()->LineTraceSingleByChannel(OutHit, m_vEndPosition, m_vEndPosition - FVector(0.f, 0.f, 100.f), ECollisionChannel::ECC_Visibility);
			if (!OutHit.bBlockingHit)
			{
				m_vEndPositionAfterAdjustment = m_vEndPosition + m_vMovementDirection * m_fDashDistanceVariation;
				GetWorld()->LineTraceSingleByChannel(OutHit, m_vEndPositionAfterAdjustment, m_vEndPositionAfterAdjustment - FVector(0.f, 0.f, 100.f), ECollisionChannel::ECC_Visibility);
				if (OutHit.bBlockingHit)
				{
					m_fTimeToResetDashDuration += m_fDashDistanceVariation / m_fDashSpeed;
				}
				else
				{
					m_vEndPositionAfterAdjustment = m_vEndPosition - m_vMovementDirection * m_fDashDistanceVariation;
					GetWorld()->LineTraceSingleByChannel(OutHit, m_vEndPositionAfterAdjustment, m_vEndPositionAfterAdjustment - FVector(0.f, 0.f, 100.f), ECollisionChannel::ECC_Visibility);
					if (OutHit.bBlockingHit)
					{
						m_fTimeToResetDashDuration -= m_fDashDistanceVariation / m_fDashSpeed;
					}
				}
			}
		}
	}
}

void APlayerCharacter::Bat()
{
	if (!m_bIsBatRunning && !m_bIsStunned) // When bat is started
	{
		OnBat(); // Event for blueprint use
		if (m_bIsBatInCooldown) return;
		PlayAnimMontage(m_UBatAnimMontage,2.0f,NAME_None);
		
		m_USndManager->PlayEventAtLocation(GetWorld(), m_USwingFMODEvent, GetActorLocation());
		m_bIsBatRunning = true;
		m_bIsBatInCooldown = true;
		m_fTimeToResetDuration = m_fBatDuration;
		m_fTimeToResetCooldown = m_fCurrentBatCooldown;
		if (m_bIsDebugOn) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Attemp bat!"));	
	}
	// Bat
	SetActorRotation(UKismetMathLibrary::MakeRotFromX(m_vBatDirection));
	TArray<FHitResult> hits;
	UKismetSystemLibrary::SphereTraceMulti(GetWorld(),GetActorLocation(),GetActorLocation(),m_fBatMaxLength,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel6),false,
		TArray<AActor*>(),EDrawDebugTrace::ForDuration,hits,true, FLinearColor::Red,
		FLinearColor::Green, 0.2f);
	for (FHitResult hit : hits)
	{	
		AActor* aHit = hit.GetActor();
		if (!aHit->Implements<UIBateable>()) continue;
		if (!isInsideArea(aHit,m_fBatMinLength,m_fBatAngleCos)) continue;
		if (aHit->GetOwner() == this) continue;
		FHitResult outHit;
		TArray<AActor*> ignore = TArray<AActor*>();
		ignore.Add(aHit);
		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(),GetActorLocation(),hit.Location,
			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1),false,
			ignore,EDrawDebugTrace::None,outHit,true))
		{
			continue;
		}
		
 		if (m_bIsDebugOn) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("BAT"));	
		AProjectile* aProjectile = Cast<AProjectile>(aHit);
		if (aProjectile != nullptr)
		{ // A projectile is bat
			aProjectile->AddTag(m_eTag);
			if(m_iComboLevel >= m_iComboAmountToHaveElectricBullets)
			{
				m_USndManager->PlayEventAtLocation(GetWorld(), m_UBatSpecialSuccessFMODEvent, GetActorLocation());
				aProjectile->AddTag(EBulletModifier::Electric);
			}
			if (m_iCurrentTagAmmo > 0 && --m_iCurrentTagAmmo == 0)
			{
				m_eTag = EBulletModifier::None;
				OnRunOutOfTagAmmo();
			}
			m_UPlayerHUD->SetCurrentAmmo(m_iCurrentTagAmmo);

			// Check Min Bat Speed
			if (aProjectile->m_fSpeed < m_fBatSpeed + m_fBatSpeed * m_iComboLevel * m_fBulletSpeedIncreaseMultiplierPerCombo)
			{
				aProjectile->m_fSpeed = m_fBatSpeed + m_fBatSpeed * m_iComboLevel * m_fBulletSpeedIncreaseMultiplierPerCombo;
			}
			
			m_fTimeToResetCombo = m_fComboDuration;
			AddCombo(m_fComboGainedBat);
			SetComboLevel(m_iComboLevel+1);
			m_UPlayerHUD->SetCurrentComboLvl(m_iComboLevel);
			
		}
		IIBateable::Execute_GetHit(aHit,m_vBatDirection,this,true,m_fAutoaimRange,m_fAutoaimAngle);

		// m_USndManager->PlayEventAtLocation(GetWorld(), m_UBatSuccessFMODEvent, GetActorLocation());
	}
}

void APlayerCharacter::Cut()
{
	
	if (!m_bIsCutRunning) // When cut is started
	{
		const int iCutLevel = CheckComboCutLevel(m_fCurrentCutCost);

		if(iCutLevel <= 0)
			return;
		
		OnPlayerCut();
		
		if (m_bIsCutInCooldown) return;
		TArray<FHitResult> hits;
		
		if (!UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(),GetActorLocation(),GetActorLocation(), m_TCutFinalRadiuses[iCutLevel - 1],
			m_TObjectTypesHitByCut,false,
			m_TToIgnoreDuringCut, m_bIsDebugOn ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,hits,true)) return;
		
		PlayAnimMontage(m_UCutAnimMontage,2.0f,NAME_None);
		if (m_OnPlayerAction.IsBound())
			m_OnPlayerAction.Broadcast(EPlayerActionType::Cut);
		m_USndManager->PlayEventAtLocation(GetWorld(), m_UCutFMODEvent, GetActorLocation());
		
		UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_UCutParticleSystems[iCutLevel - 1],
			GetActorLocation() + FVector::UpVector * 10.0f,FRotator::ZeroRotator,FVector::OneVector,true,true,
			ENCPoolMethod::AutoRelease, true);
		SpawnedEffect->SetFloatParameter("CutInnerRadius", (m_fCutKnockBackRadius+150) * 2);
		SpawnedEffect->SetFloatParameter("CutOutRadius", (m_TCutFinalRadiuses[iCutLevel - 1]+500) * 2);
		
		m_bIsCutRunning = true;
		m_bIsCutInCooldown = true;
		m_fTimeToResetCutDuration = m_fCutTotalDuration;
		m_fTimeToResetCutCooldown = m_fCutCooldown;
		m_fCutExpansionProgress = 0.0f;
		m_iCurrentCutLevel = iCutLevel;
		
		UseCombo(m_fCurrentCutCost * iCutLevel);
		
		if (m_bIsDebugOn) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Attemp cut!"));	
	}
	if(m_bIsCutRunning)
	{
		const float fCutKnockBackRadiusSquared = m_fCutKnockBackRadius * m_fCutKnockBackRadius;
		m_fCutExpansionProgress = FMath::Clamp(m_fCutExpansionProgress + GetWorld()->DeltaTimeSeconds / m_fCutExpansionDuration, 0.0f, 1.0f);
		const float fCurrentCutRadius = FMath::Lerp(m_fCutStartingRadius, m_TCutFinalRadiuses[m_iCurrentCutLevel - 1], m_fCutExpansionProgress);

		UKismetSystemLibrary::PrintString(GetWorld(), FString::FromInt(fCurrentCutRadius));
		
		// Cut
		TArray<FHitResult> hits;
		bool isAnyHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(),GetActorLocation(),GetActorLocation(),fCurrentCutRadius,
			m_TObjectTypesHitByCut,false,
			m_TToIgnoreDuringCut, m_bIsDebugOn ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,hits,true,
			fCurrentCutRadius < m_fCutKnockBackRadius ? FLinearColor::Green : FLinearColor::Red, FLinearColor::Green, 0.2f);
		
		for (FHitResult hit : hits)
		{
			AActor* aHit = hit.GetActor();
			if (aHit->GetOwner() == this) continue;
			IIBateable* IBateable = Cast<IIBateable>(aHit);
			const FVector VHitDirection = (aHit->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			if (IBateable != nullptr)
			{
				if(m_iCurrentCutLevel >= 3){
					AProjectile* ABullet = Cast<AProjectile>(aHit);
					if(ABullet)
					{
						ABullet->AddTag(EBulletModifier::Electric);
					}
				}
				IBateable->Execute_GetHit(aHit, VHitDirection, this, true, m_fAutoaimRange, m_fAutoaimAngle);
			}
			else
			{
				ABaseEnemy* ANotExplosiveEnemy = Cast<ABaseEnemy>(aHit);
				if(FVector::DistSquared(aHit->GetActorLocation(), GetActorLocation()) <= fCutKnockBackRadiusSquared)
				{
					//KnockBack y Stun
					const float fKnockBackDistance = (m_fCutKnockBackRadius + 100.0f) - FVector::Dist(aHit->GetActorLocation(), GetActorLocation());
					ANotExplosiveEnemy->KnockBack(VHitDirection, m_fCutKnockBackSpeed, fKnockBackDistance, m_fCutStunDuration);
				} else
				{
					//Stun
					ANotExplosiveEnemy->KnockBack(FVector::ZeroVector, 0.0f, 0.0f, m_fCutStunDuration);
				}
			}

			m_TToIgnoreDuringCut.Add(aHit);

			if (m_bIsDebugOn) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CUT"));

			// Cut particles
			// UNiagaraComponent* cutParticles = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),m_UCutParticleSystem,
			// 	aProjectile->GetActorLocation(),FRotator::ZeroRotator,FVector::OneVector,true,true,
			// 	ENCPoolMethod::AutoRelease,true);
			// cutParticles->ActivateSystem(true);
			
			// aHit->Destroy();
			
		}
	}
}

void APlayerCharacter::ToggleControllerWidget()
{
	mc_WControllerToggle->SetVisibility(m_bIsControllerWidgetVisible);
}

void APlayerCharacter::ToggleController()
{
	m_bIsUsingController = !m_bIsUsingController;
	UToggleController* pToggle = Cast<UToggleController>(mc_WControllerToggle->GetUserWidgetObject());
	if (pToggle)
	{
		pToggle->m_bUseControllerUI = m_bIsUsingController;
		pToggle->ControllerToggleWarning();
	}
	if (m_bIsDebugOn) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		FString::Printf(TEXT("Controller mode: %o"), m_bIsUsingController));

	m_bIsControllerWidgetVisible = true;
	ToggleControllerWidget();
	m_bIsControllerWidgetVisible = false;
	m_fTimeToResetControllerHUD = 2.0f;
}

void APlayerCharacter::ChangeTag(EBulletModifier _ENewTag)
{
	m_iCurrentTagAmmo = m_iMaxTagAmmo;
	m_eTag = _ENewTag;
	m_UPlayerHUD->SetCurrentAmmo(m_iCurrentTagAmmo);
	m_UPlayerHUD->ChangeAmmoTag(_ENewTag);
}

void APlayerCharacter::ChangeDamageImmunityColor(FColor _FColor)
{
	for (UMaterialInstanceDynamic* UMeshMaterial : m_UMeshMaterials)
	{
		if(IsValid(UMeshMaterial))
		{
			UMeshMaterial->SetVectorParameterValue("BlinkColor", _FColor);
		}
	}
}

void APlayerCharacter::ActivateDamageImmunity(float _fActive)
{
	for(UMaterialInstanceDynamic* UMeshMaterial : m_UMeshMaterials)
	{
		if(IsValid(UMeshMaterial))
		{
			UMeshMaterial->SetScalarParameterValue("Active", _fActive);
		}
	}
}

void APlayerCharacter::OnDamagedCallback(float _fCurrentHealth)
{
	ActivateDamageImmunity(1.0f);
	ChangeDamageImmunityColor(m_FDamagedRimColor);
	m_UPlayerHUD->SetCurrentHealth(_fCurrentHealth);

	m_bIsImmune = true;

	m_fTimeToResetCombo = 0.0f;
	SetComboLevel(0);

	if(_fCurrentHealth > 0.0f)
	{
		m_USndManager->PlayEventAtLocation(GetWorld(),m_UDamagedFMODEvent,GetActorLocation());
	}

	OnPlayerDamaged();
}

void APlayerCharacter::OnEndDamagedCallback()
{
	ChangeDamageImmunityColor(m_FImmunityRimColor);
}

void APlayerCharacter::OnEndImmunityCallback()
{
	ActivateDamageImmunity(0.0f);
	m_bIsImmune = false;
}

void APlayerCharacter::OnHealCallback(float _fCurrentHealth)
{
	m_USndManager->PlayEventAtLocation(GetWorld(), m_UHealFMODEvent, GetActorLocation());
	
	if(mc_UHealedNiagaraComponent->IsComplete())
		mc_UHealedNiagaraComponent->Activate(true);
	
	m_UPlayerHUD->SetCurrentHealth(_fCurrentHealth);
}

void APlayerCharacter::OnDeathCallback()
{
	ActivateDamageImmunity(1.0f);
	ChangeDamageImmunityColor(m_FDamagedRimColor);
	
	m_USndManager->PlayEventAtLocation(GetWorld(), m_UDeathFMODEvent, GetActorLocation());
	
	m_fCurrentMovementSpeed = m_fMovementSpeed;
	GetCharacterMovement()->MaxWalkSpeed = m_fCurrentMovementSpeed;
	
	AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (pGM)
	{
		//pGM->OnPlayerKilled.Broadcast();
		pGM->RespawnCharacter(this);
		m_UPlayerHUD->SetCurrentHealth(mc_UHealth->GetHealth());
	}
	else {
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in healthcomponent: Could not cast to Umbra gamemode"));
#endif 
		UE_LOG(LogTemp, Error, TEXT("Error in healthComponent: Could not cast to Umbra gamemode"));
	}
}

void APlayerCharacter::OnHealthChangedCallback(float _fCurrentHealth)
{
	m_UPlayerHUD->SetCurrentHealth(_fCurrentHealth);
}

void APlayerCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	if(IsValid(m_UPlayerHUD))
		m_UPlayerHUD->RemoveFromParent();
}

void APlayerCharacter::SetComboAmount(float _fComboAmount)
{
	m_fCombo = _fComboAmount;
	m_UPlayerHUD->SetCurrentComboPercent(m_fCombo/m_fComboMax);
}

void APlayerCharacter::OnAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	mc_UHealth->TakeDamage(Damage);
}

void APlayerCharacter::OnStartFalling(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (!m_bIsDashing)
	{
		if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
		{
			m_bCoyoteTimeActive = true;
			GetCharacterMovement()->GravityScale = 0.f;
			m_fTimeToResetCoyoteTime = m_fCoyoteTimeDuration;
		}
	}
}

const UHealthComponentCpp* APlayerCharacter::GetHealthComponent()
{
	return mc_UHealth;
}

void APlayerCharacter::DrawAimArea()
{
	if (!m_bIsAiming && m_bAlphaWhenNotAiming && m_bIsUsingController)
	{
		m_UBatAreaMaterialDynamic->SetScalarParameterValue(FName("ConeVisibility"), 0.0f);
	}
	else
	{
		if (m_bIsBatInCooldown)
		{
			m_UBatAreaMaterialDynamic->SetVectorParameterValue(FName("Color"), m_FDebugColor);
			m_UBatAreaMaterialDynamic->SetScalarParameterValue(FName("ConeVisibility"), m_fConeAlphaInactive);
		}
		else {
			if (m_bIsBatRunning)
			{
				m_UBatAreaMaterialDynamic->SetVectorParameterValue(FName("Color"), m_FDebugColor);
				m_UBatAreaMaterialDynamic->SetScalarParameterValue(FName("ConeVisibility"), m_fConeAlphaInactive);
			}
			else {
				m_UBatAreaMaterialDynamic->SetVectorParameterValue(FName("Color"), m_FDebugColor);
				m_UBatAreaMaterialDynamic->SetScalarParameterValue(FName("ConeVisibility"), m_fConeAlphaActive);

			}
		}
	}
}

void APlayerCharacter::RotateWithMouse()
{
	if (Controller && Controller->IsPlayerController())
	{
		FHitResult hit;
		if (Cast<APlayerController>(Controller)->GetHitResultUnderCursorByChannel(
			UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel9),false,hit)) // ECC_GameTraceChannel9 = MouseAim
				{
					m_vBatDirection = hit.Location - GetActorLocation();
					m_vBatDirection.SetComponentForAxis(EAxis::Z,0.0f);
					m_vBatDirection.Normalize();
					
					m_fTimeToResetAiming = m_fLastJoystickMem;
					m_bUseMovementAim = false;
				} 
	}
}

void APlayerCharacter::RotateWithController()
{
	if (sqrt(m_fLookAtControllerV*m_fLookAtControllerV + m_fLookAtControllerH*m_fLookAtControllerH)
		>= m_fRightCenterDeadzone) // if LookAtController OUTside the center deadzone
		{
			m_bIsAiming = true;
			if (m_fLookAtControllerV != 0.0f || m_fLookAtControllerH != 0.0f)
			{
				FVector vCamForward = m_ACamera->GetCameraComponent()->GetForwardVector();
				vCamForward.SetComponentForAxis(EAxis::Z,0.0f);
				FVector vCamRight = vCamForward.RotateAngleAxis(90,vCamForward.ZAxisVector);
				vCamRight.SetComponentForAxis(EAxis::Z,0.0f);
				m_vBatDirection = (vCamForward * m_fLookAtControllerV
										+ vCamRight * m_fLookAtControllerH);
				m_vBatDirection.Normalize();
				
				m_fTimeToResetAiming = m_fLastJoystickMem;
				m_bUseMovementAim = false;
			}
		} else
		{
			m_bIsAiming = false;
		}
}

FRotator APlayerCharacter::GetMouseRotation(FVector _vHitLocation)
{
	return UKismetMathLibrary::FindLookAtRotation(
		FVector(GetActorLocation().X,GetActorLocation().Y,0.0f),FVector(_vHitLocation.X,_vHitLocation.Y,0.0f));
}

void APlayerCharacter::Aim()
{
	if (m_bIsUsingController) RotateWithController(); else RotateWithMouse();
	if (m_bUseMovementAim) m_vBatDirection = m_vMovementDirection;
	//	mc_UAimConeDecal->SetWorldRotation(m_vMovementDirection.Rotation() + FRotator(0.0f,90.0f,0.0f)); // 90º Offset due to shader in V instead of U texcoord
	//else
		mc_UAimConeDecal->SetWorldRotation(m_vBatDirection.Rotation() + FRotator(90.0f,90.0f,0.0f)); // 90º yaw Offset due to shader in V instead of U texcoord. 90º pitch because decal rotation.
		mc_UAimMeshPlane->SetWorldRotation(m_vBatDirection.Rotation() + FRotator(00.0f,90.0f,0.0f)); // 90º yaw Offset due to shader in V instead of U texcoord.
	
}

void APlayerCharacter::Move(float _fDeltaTime)
{
	if (m_bIsStunned)
	{
		const TArray<AActor*> TActorsToIgnore;
		FHitResult FHitResult;
		const FVector vCurrentLocation = GetActorLocation();
		//Gets the correct deltaTime for the Knockback this frame based on the value of the remaining duration for the knockback
		//This way the distance traveled by the Knockback will never be more than the max (duration * speed)
		const float fCorrectDeltaTime = m_fKnockbackDuration >= 0 ? _fDeltaTime : _fDeltaTime + m_fKnockbackDuration;
		FVector vFinalLocation = vCurrentLocation + m_vMovementDirection * GetCharacterMovement()->MaxWalkSpeed * fCorrectDeltaTime;
		
		if(UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			vCurrentLocation,
			vFinalLocation,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TActorsToIgnore,
			EDrawDebugTrace::None,
			FHitResult,
			true
			)
		)
		{
			vFinalLocation = FHitResult.ImpactPoint - m_vMovementDirection * GetCapsuleComponent()->GetUnscaledCapsuleRadius();
		}
		
		SetActorLocation(vFinalLocation);
		return;
	};
	if (sqrt(m_fMoveToControllerV*m_fMoveToControllerV + m_fMoveToControllerH*m_fMoveToControllerH)
		< m_fLeftCenterDeadzone) return; // if LookAtController INside the center deadzone. RETURN
	if (m_fMoveToControllerV != 0.0f || m_fMoveToControllerH != 0.0f)
	{
		FVector vCamForward = m_ACamera->GetCameraComponent()->GetForwardVector();
		vCamForward.SetComponentForAxis(EAxis::Z,0.0f);
		FVector vCamRight = vCamForward.RotateAngleAxis(90,vCamForward.ZAxisVector);
		vCamRight.SetComponentForAxis(EAxis::Z,0.0f);
		m_vMovementDirection = (vCamForward * m_fMoveToControllerV
								+ vCamRight * m_fMoveToControllerH);
		m_vMovementDirection.Normalize();
		AddMovementInput(m_vMovementDirection);
		SetActorRotation(m_vMovementDirection.Rotation());
	}
	
}

/**
 * WARNING: Bad name for this function. Not generic.
 * TODO: Change it to prevent [more] confusions.
 */
bool APlayerCharacter::isInsideArea(AActor* _AActor, float _fMinLength, float _fAngleCos)
{
	FVector vBulletLoc = _AActor->GetActorLocation();
	FVector vPlayerLoc = GetActorLocation();
	vBulletLoc.Z = vPlayerLoc.Z;
	FVector DirNorm = (vBulletLoc - vPlayerLoc);
	UKismetMathLibrary::Vector_Normalize(DirNorm);
	float Distance = UKismetMathLibrary::Vector_Distance(vBulletLoc, vPlayerLoc);
	float DotRes = UKismetMathLibrary::Dot_VectorVector(DirNorm, m_vBatDirection);

	return ((Distance>_fMinLength) && (DotRes>_fAngleCos));
}

void APlayerCharacter::GetReturnDirection(AProjectile _UProjectile, FVector _vDirection)
{
}

float APlayerCharacter::GetNearestAngle(const FVector& _vFrom, const FVector& _vTo)
{
	// angle = acosd(from * to) * zSign(from x to)
	return UKismetMathLibrary::DegAcos(FVector::DotProduct(_vFrom, _vTo))
				* FVector::CrossProduct(_vFrom, _vTo).GetSignVector().Z;
}

void APlayerCharacter::Interact()
{
	if (m_TTagDrops.Num() > 0)
	{
		InteractWithNearestInteractable();
	}
}

void APlayerCharacter::InteractWithNearestInteractable()
{
	TWeakObjectPtr<ATagDrop> pNearest=nullptr;
	for (TWeakObjectPtr<ATagDrop> i : m_TTagDrops)
	{
		if (pNearest.IsValid())
		{
			const float fDistanceToNearest = UKismetMathLibrary::Vector_Distance(pNearest->GetActorLocation(), GetActorLocation());
			const float fDistanceToNext = UKismetMathLibrary::Vector_Distance(GetActorLocation(), i->GetActorLocation());
			if (fDistanceToNearest > fDistanceToNext)
			{
				pNearest = i;
			}
		}
		else {
			pNearest = i;
		}
	}
	pNearest->Collect(this);
}

void APlayerCharacter::SetUpBatAreaEffect()
{
	mc_UAimConeDecal->DecalSize = FVector(m_fConeHeight, m_fBatMaxLength + m_fBatMaxLengthVisualOffset, m_fBatMaxLength);
	m_UBatAreaMaterialDynamic = mc_UAimConeDecal->CreateDynamicMaterialInstance();
	m_UBatAreaMaterialDynamic->SetScalarParameterValue(FName("MinLength"), (m_fBatMinLength / m_fBatMaxLength) / 2.f);
	m_UBatAreaMaterialDynamic->SetScalarParameterValue(FName("Angle"), UKismetMathLibrary::DegreesToRadians(m_fBatAngle + m_fBatAngleVisualOffset));

	mc_UAimMeshPlane->SetWorldScale3D(FVector((m_fBatMaxLength + m_fBatMaxLengthVisualOffset)/50.0f));
	m_UMiniBatAreaMaterialDynamic = mc_UAimMeshPlane->CreateDynamicMaterialInstance(0, mc_UAimMeshPlane->GetMaterial(0));
	m_UMiniBatAreaMaterialDynamic->SetScalarParameterValue(FName("MinLength"), (m_fBatMinLength / m_fBatMaxLength) / 2.f);
	m_UMiniBatAreaMaterialDynamic->SetScalarParameterValue(FName("Angle"), UKismetMathLibrary::DegreesToRadians(m_fBatAngle + m_fBatAngleVisualOffset));
}

void APlayerCharacter::AddCombo(float _fAmount)
{
	float fFinalAmount;
	
	if (m_iComboLevel > m_iMaxLvlCombo) fFinalAmount = _fAmount + (m_iMaxLvlCombo * m_fComboLvlMultiplier);
	else fFinalAmount = _fAmount + (m_iComboLevel * m_fComboLvlMultiplier);
	const float fFinalComboTotalAmount = m_fCombo + fFinalAmount;

	if(m_fCombo < m_fCutComboCost && fFinalComboTotalAmount >= m_fCutComboCost)
		m_USndManager->PlayEventAtLocation(GetWorld(), m_UCutComboChargedFMODEvent, GetActorLocation());

	// if(m_fCombo < m_fSpecialComboCost && fFinalComboTotalAmount >= m_fSpecialComboCost)
	// 	m_USndManager->PlayEventAtLocation(GetWorld(), m_USpecialComboChargedFMODEvent, GetActorLocation());

	m_fCombo = fFinalComboTotalAmount;
	
	if (m_fCombo > m_fComboMax) m_fCombo = m_fComboMax;
	

	m_UPlayerHUD->SetCurrentComboPercent(m_fCombo/m_fComboMax);
	
	if (m_bIsDebugOn) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			FString::Printf(TEXT("Gained %f of Combo (Combo lvl: %o). %f left."),
				fFinalAmount, m_iComboLevel, m_fCombo));
}

void APlayerCharacter::SetComboLevel(int _iNewComboLevel)
{
	m_iComboLevel = _iNewComboLevel;
	m_iComboLevel = FMath::Clamp(m_iComboLevel, 0, m_iMaxLvlCombo);

	if(!m_USpecialAudioComponent->IsPlaying() && m_iComboLevel >= m_iComboAmountToHaveElectricBullets)
	{
		mc_UElectricTagNiagaraComponent->Activate(true);
		m_USpecialAudioComponent->Play();
	}
	else if(m_iComboLevel < m_iComboAmountToHaveElectricBullets)
	{
		mc_UElectricTagNiagaraComponent->Deactivate();
		m_USpecialAudioComponent->Stop();
	}
}

bool APlayerCharacter::UseCombo(float _fAmount)
{
	if (m_fCombo >= _fAmount)
	{
		m_fCombo -= _fAmount;
		m_UPlayerHUD->SetCurrentComboPercent(m_fCombo/m_fComboMax);
		if (m_bIsDebugOn) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			FString::Printf(TEXT("Used %f of Combo. %f left."), _fAmount, m_fCombo));
		return true;
	}
	else
	{
		if (m_bIsDebugOn) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			FString::Printf(TEXT("No enough Combo (%f). %f left."), _fAmount, m_fCombo));
		return false;
	}
}

bool APlayerCharacter::CheckCombo(float _fAmount)
{
	if (m_fCombo >= _fAmount) return true; return false;
}

int APlayerCharacter::CheckComboCutLevel(float _fAmount)
{
	int iFinalLevel = 0;

	while(iFinalLevel < m_iMaxLevelCutUsable)
	{
		if(m_fCombo >= _fAmount * (iFinalLevel + 1))
		{
			++iFinalLevel;
		}
		else
		{
			return iFinalLevel;
		}
	}

	return iFinalLevel;
}

