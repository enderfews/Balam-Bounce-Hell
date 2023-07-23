// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Automatinator/ActionRecorderComponent.h"
#include "ProjectUmbra/CustomDataType/BulletLUT.h"
#include "PlayerCharacter.generated.h"

class UFMODAudioComponent;
class USoundManager;
class UFMODEvent;
class UNiagaraComponent;
class UNiagaraSystem;
class UPlayerHUD;
class UWidget;
class AProjectile;
class UWidgetComponent;
class UAIPerceptionStimuliSourceComponent;
class UHealthComponentCpp;
class UDamageType;
class UMaterial;
class AController;
class UMaterialInstanceDynamic;
class ATagDrop;
class UForceFeedbackEffect;
class UDecalComponent;
class UDeathRecorderComponent;
class UPathRecorderComponent;
UCLASS()
class PROJECTUMBRA_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	
	// * Private Components
	UPROPERTY()
	TArray<UNiagaraComponent*> mc_UCutLevelNiagaraComponents;
	UPROPERTY()
	UNiagaraComponent* mc_UDashNiagaraComponent;
	UPROPERTY()
	UNiagaraComponent* mc_UHealedNiagaraComponent;
	UPROPERTY()
	UNiagaraComponent* mc_USpecialNiagaraComponent;
	UPROPERTY()
	UNiagaraComponent* mc_UElectricTagNiagaraComponent;

	// * Private Variables
	// Movement
	float m_fCurrentMovementSpeed;
	float m_fSlowDuration;
	bool m_bIsGrounded;

	//Knockback
	float m_fKnockbackDuration;
	float m_fStunDuration;
	
	// Debug
	float m_fChangeSpeed;
	FVector m_vDebugForward;
	FVector m_vDebugRight;
	FLinearColor m_FDebugColor;
	UPROPERTY()
	UWidget* m_WDebugUI; // TODO: DebugUI_PROTO
	UPROPERTY()
	UMaterialInstanceDynamic* m_UBatAreaMaterialDynamic;
	UPROPERTY()
	UMaterialInstanceDynamic* m_UMiniBatAreaMaterialDynamic;
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> m_UMeshMaterials;
	
	// Bat
	float m_fBatAngleCos;
	float m_fTimeToResetDuration;
	float m_fTimeToResetCooldown;
	bool m_bIsBatInCooldown;
	bool m_bIsBatRunning;
	int m_iCurrentTagAmmo = 0;
	float m_fCurrentBatCooldown;

	// Dash
	bool m_bIsDashing;
	bool m_bCanDash;
	bool m_bIdle;
	bool m_bDashStart;
	bool m_bCoyoteTimeActive;
	float m_fTimeToResetDashDuration;
	float m_fTimeToResetDashCooldown;
	float m_fTimeToResetDashImmunityTime;
	float m_fTimeToResetCoyoteTime;
	float m_fBrakingFrictionFactor;
	float m_fBrakingDecelerationWalking;
	float m_fGravityScale;
	FVector m_vVelocity;
	FVector m_vEndPosition;
	FVector m_vEndPositionAfterAdjustment;
	float m_fCurrentDashCooldown;

	// Controls
	FVector m_vLastJoystickPos;
	float m_fTimeToResetAiming;
	bool m_bUseMovementAim;
	bool m_bIsAiming;

	// Configuration
	bool m_bIsStunned;

	// Cut
	bool m_bIsCutInCooldown = false;
	bool m_bIsCutRunning = false;
	float m_fTimeToResetCutDuration = 0.0f;
	float m_fTimeToResetCutCooldown = 0.0f;
	float m_fCutExpansionProgress = 0.0f;
	UPROPERTY()
	TArray<AActor*> m_TToIgnoreDuringCut;
	int m_iCurrentCutLevel = 0;

	//TagDrops
	UPROPERTY()
	TArray<ATagDrop*> m_TTagDrops;

	// Combo (Energy bar)
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	float m_fCombo;
	bool m_bIsComboRunning;
	int m_iComboLevel;
	float m_fTimeToResetCombo;

	// Others
	bool m_bIsControllerWidgetVisible;
	float m_fTimeToResetControllerHUD;
	UPROPERTY(BlueprintReadOnly, DisplayName="Is Immune", meta = (AllowPrivateAccess = "true"))
	bool m_bIsImmune = false;

	//UI
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPlayerHUD* m_UPlayerHUD;

	//Sounds
	UPROPERTY()
	USoundManager* m_USndManager;
	UPROPERTY()
	UFMODAudioComponent* m_USpecialAudioComponent;
	

	// * Private Methods

	// Movement
	void RotateWithMouse();
	void RotateWithController();
	FRotator GetMouseRotation(FVector _vHitLocation);
	void Move(float _fDeltaTime);
	void MoveForwardAxis(float Value);
	void MoveRightAxis(float Value);
	void RotateForwardAxis(float Value);
	void RotateRightAxis(float Value);
	void CheckGrounded();

	// Bat
	void Aim();
	void DrawAimArea();
	bool isInsideArea(AActor* _AActor, float _fMinLength, float _fAngleCos);
	void GetReturnDirection(AProjectile _UProjectile, FVector _vDirection);

	// Combo
	bool UseCombo(float _fAmount);
	void AddCombo(float _fAmount);
	bool CheckCombo(float _fAmount);
	int CheckComboCutLevel(float _fAmount);

	// Utils
	static float GetNearestAngle(const FVector& _vFrom, const FVector& _vTo);

	// Others
	void Interact();
	void InteractWithNearestInteractable();
	void SetUpBatAreaEffect();
	void UpdateTimers(float _fDeltaTime);
	
	//Damage Visual Feedback
	void ChangeDamageImmunityColor(FColor _FColor);
	void ActivateDamageImmunity(float _fActive);

	//Health Callbacks
	UFUNCTION()
	void OnDamagedCallback(float _fCurrentHealth);
	UFUNCTION()
	void OnEndDamagedCallback();
	UFUNCTION()
	void OnEndImmunityCallback();
	UFUNCTION()
	void OnHealCallback(float _fCurrentHealth);
	UFUNCTION()
	void OnDeathCallback();
	UFUNCTION()
	void OnHealthChangedCallback(float _fCurrentHealth);
	UFUNCTION()
	virtual void BeginDestroy() override;

protected:
	// * Protected Components
	
	// * Protected Variables

	// * Protected Methods
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// * Public Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="ControllerWidget"))
	UWidgetComponent* mc_WControllerToggle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="ControlsWidget"))
	UWidgetComponent* mc_WControls;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="HealthBarWidget"))
	UWidgetComponent* mc_WHealthBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="InteractPopUpWidget"))
	UWidgetComponent* mc_UInteractPopUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="AimDecal"))
	UDecalComponent* mc_UAimConeDecal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="AimMeshPlane"))
	UStaticMeshComponent* mc_UAimMeshPlane;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="MouseAimMesh"))
	UStaticMeshComponent* mc_UMouseAimPlane;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="MovArrowMesh"))
	UStaticMeshComponent* mc_UMovArrowPlane;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="StimuliSource"))
	UAIPerceptionStimuliSourceComponent* mc_UStimuliSource;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta=(DisplayName="HealthComponent"))
	UHealthComponentCpp* mc_UHealth;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (DisplayName = "Death Recorder"))
		UDeathRecorderComponent* mc_UDeathRecorder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (DisplayName = "Path Recorder"))
		UPathRecorderComponent* mc_UPathRecorder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (DisplayName = "Action Recorder"))
		UActionRecorderComponent* mc_UActionRecorder;
	// * Public Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta=(DisplayName="MovementSpeed"))
	float m_fMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta=(DisplayName="LookAtControllerV"))
	float m_fLookAtControllerV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta=(DisplayName="LookAtControllerH"))
	float m_fLookAtControllerH;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta=(DisplayName="MoveToControllerV"))
	float m_fMoveToControllerV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta=(DisplayName="MoveToControllerH"))
	float m_fMoveToControllerH;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta=(DisplayName="Grounded check height"))
	float m_fGroundedCheckHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta=(DisplayName="IsDebugOn"))
	bool m_bIsDebugOn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry recorder", meta = (DisplayName = "Activate recording data"))
		bool m_bActivateRecordData;
	//Public for dbugging purposes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta=(DisplayName="IsDebugChanged"))
	bool m_bIsDebugChanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta=(DisplayName="MaxLengthChange"))
	float m_fMaxLengthChange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta=(DisplayName="MinLengthChange"))
	float m_fMinLengthChange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta=(DisplayName="AngleChange"))
	float m_fAngleChange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta=(DisplayName="ConeAlphaActive"))
	float m_fConeAlphaActive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta=(DisplayName="ConeAlphaInactive"))
	float m_fConeAlphaInactive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta=(DisplayName="ConeHeight"))
	float m_fConeHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat", meta=(DisplayName="BatCooldown"))
	float m_fBatCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat", meta=(DisplayName="BatDuration"))
	float m_fBatDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat", meta=(DisplayName="BatMaxLength"))
	float m_fBatMaxLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat", meta=(DisplayName="BatMinLength"))
	float m_fBatMinLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat", meta=(DisplayName="BatAngle"))
	float m_fBatAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat", meta=(DisplayName="AutoaimRange"))
	float m_fAutoaimRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat", meta=(DisplayName="AutoaimAngle"))
	float m_fAutoaimAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat", meta = (DisplayName = "Use forward bat"))
	bool m_bIsUsingForwardBat; // TODO: Only implement Forward Bat.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat", meta = (DisplayName = "Max Tag Ammo"))
	int m_iMaxTagAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat", meta=(DisplayName="Bat Speed"))
	float m_fBatSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat", meta=(DisplayName="Bat Angle Visual Offset"))
	float m_fBatAngleVisualOffset = -10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bat", meta=(DisplayName="Bat Max Length Visual Offset"))
	float m_fBatMaxLengthVisualOffset = -10.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta=(DisplayName="DashSpeed"))
	float m_fDashSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta=(DisplayName="DashRate"))
	float m_fDashRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta=(DisplayName="DashDuration"))
	float m_fDashDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta=(DisplayName="ImmunityTime"))
	float m_fImmunityTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta=(DisplayName="DashDistanceVariation"))
	float m_fDashDistanceVariation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta = (DisplayName = "Coyote Time Duration"))
	float m_fCoyoteTimeDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", meta = (DisplayName = "Dash Z Velocity"))
	float m_fDashZVelocity;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls", meta=(DisplayName="IsUsingController"))
	bool m_bIsUsingController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls", meta=(DisplayName="LeftCenterDeadzone"))
	float m_fLeftCenterDeadzone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls", meta=(DisplayName="RightCenterDeadzone"))
	float m_fRightCenterDeadzone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls", meta=(DisplayName="LastJoystickMem"))
	float m_fLastJoystickMem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Controls", meta=(DisplayName="Alpha when not aiming"))
	bool m_bAlphaWhenNotAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", meta=(DisplayName="Camera"))
	ACameraActor* m_ACamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", meta=(DisplayName="InGodMode"))
	bool m_bInGodMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", meta=(DisplayName="InTestMode"))
	bool m_bInTestMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration", meta = (DisplayName = "Tag"))
	EBulletModifier m_eTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cut", meta = (DisplayName = "Cut Cooldown"))
	float m_fCutCooldown = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cut", meta = (DisplayName = "Cut Total Duration"))
	float m_fCutTotalDuration = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cut", meta = (DisplayName = "Cut Cut Expansion Duration"))
    float m_fCutExpansionDuration = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cut", meta = (DisplayName = "Cut Starting Radius"))
	float m_fCutStartingRadius = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cut", meta = (DisplayName = "Cut Final Radiuses"))
	TArray<float> m_TCutFinalRadiuses;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cut", meta = (DisplayName = "Cut Knock Back Radius"))
	float m_fCutKnockBackRadius = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cut", meta = (DisplayName = "Cut Knock Back Speed"))
	float m_fCutKnockBackSpeed = 1500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cut", meta = (DisplayName = "Cut Stun Duration"))
	float m_fCutStunDuration = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cut", meta = (DisplayName = "Can Move During Cut"))
	bool m_bCanMoveDuringCut = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cut", meta = (DisplayName = "Object Types Hit By Cut"))
	TArray<TEnumAsByte<EObjectTypeQuery>> m_TObjectTypesHitByCut;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cut", meta = (DisplayName = "Max Level Cut Usable"))
	int m_iMaxLevelCutUsable = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (DisplayName = "Player HUD Class"))
	TSubclassOf<UUserWidget> m_TPlayerHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo", meta = (DisplayName = "Cut Combo Cost"))
	float m_fCutComboCost = 33.0f;
	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "Current Cut Combo Cost"))
	float m_fCurrentCutCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo", meta = (DisplayName = "Combo Gained with Bat"))
	float m_fComboGainedBat = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo", meta = (DisplayName = "Combo multiplier by level"))
	float m_fComboLvlMultiplier = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo", meta = (DisplayName = "Max combo level"))
	int m_iMaxLvlCombo = 7.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo", meta = (DisplayName = "Combo duration"))
	float m_fComboDuration = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo", meta = (DisplayName = "Combo Maximum"))
	float m_fComboMax = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo", meta = (DisplayName = "Combo Amount to Increase Bullet Speed"))
	float m_fBulletSpeedIncreaseMultiplierPerCombo = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo", meta = (DisplayName = "Combo Amount to Have Electric Bullets"))
	int m_iComboAmountToHaveElectricBullets = 5;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (DisplayName = "Swing FMOD Event"))
	UFMODEvent* m_USwingFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (DisplayName = "Bat Special Success FMOD Event"))
	UFMODEvent* m_UBatSpecialSuccessFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (DisplayName = "Cut FMOD Event"))
	UFMODEvent* m_UCutFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (DisplayName = "Special FMOD Event"))
	UFMODEvent* m_USpecialFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (DisplayName = "Dash FMOD Event"))
	UFMODEvent* m_UDashFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (DisplayName = "Damaged FMOD Event"))
	UFMODEvent* m_UDamagedFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (DisplayName = "Death FMOD Event"))
	UFMODEvent* m_UDeathFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (DisplayName = "Heal FMOD Event"))
	UFMODEvent* m_UHealFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (DisplayName = "Cut Combo Charged FMOD Event"))
	UFMODEvent* m_UCutComboChargedFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (DisplayName = "Special Combo Charged FMOD Event"))
	UFMODEvent* m_USpecialComboChargedFMODEvent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sounds", meta = (DisplayName = "Step FMOD Event"))
	UFMODEvent* m_UStepFMODEvent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (DisplayName = "Bat Animation"))
	UAnimMontage* m_UBatAnimMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (DisplayName = "Cut Animation"))
	UAnimMontage* m_UCutAnimMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations", meta = (DisplayName = "Dash Animation"))
	UAnimMontage* m_UDashAnimMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Info", meta = (DisplayName = "Movement Direction"))
	FVector m_vMovementDirection;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Info", meta = (DisplayName = "Aim Direction"))
	FVector m_vBatDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (DisplayName = "Cut Particle Systems"))
	TArray<UNiagaraSystem*> m_UCutParticleSystems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (DisplayName = "Dash Particle System"))
	UNiagaraSystem* m_UDashParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (DisplayName = "Healed Particle System"))
	UNiagaraSystem* m_UHealedParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (DisplayName = "Electrict Tag Particle System"))
	UNiagaraSystem* m_UElectricTagParticleSystem;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX", meta = (DisplayName = "Damaged Rim Color"))
	FColor m_FDamagedRimColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX", meta = (DisplayName = "Immunity Rim Color"))
	FColor m_FImmunityRimColor;
	
	// * Public Methods
	
	// Receive slow from bullets.
	UFUNCTION(BlueprintCallable)
	void ReceiveSlow(float _fSlowDuration, float _fSlowAmount);
	// Receive knock back from actors in the scene and apply a stun time.
	UFUNCTION(BlueprintCallable)
	void ReceiveKnockback(const FVector& _vDir, float _fTime, float _fSpeed, float _fStunTime);
	void RemoveControls(float _fDuration);
	void ActivateControls();
	// Mechanic: Dash. Move a distance at very high speed.
	UFUNCTION(BlueprintCallable)
	void Dash();
	// Mechanic: Cut. Delete all the projectiles around and in a cone.
	UFUNCTION(BlueprintCallable)
	void Cut();
	// Mechanic: Bat. Reflect all the projectiles in a cone in front of the player.
	UFUNCTION(BlueprintCallable)
	void Bat();
	UFUNCTION(BlueprintImplementableEvent,  Category = "OnBat")
	void OnBat();
	UFUNCTION(BlueprintImplementableEvent,  Category = "OnRunOutOfTagAmmo")
	void OnRunOutOfTagAmmo();
	UFUNCTION(BlueprintImplementableEvent,  Category = "OnPlayerDamaged")
	void OnPlayerDamaged();
	UFUNCTION(BlueprintImplementableEvent,  Category = "OnPlayerDash")
	void OnPlayerDash();
	UFUNCTION(BlueprintImplementableEvent,  Category = "OnPlayerCut")
	void OnPlayerCut();
	// Toggle visbiility of the Controller Widget
	void ToggleControllerWidget();
	// Toggle between controller or mouse-based aiming TODO: Do it automatically
	void ToggleController();
	//Called by tag drops when hit
	void ChangeTag(EBulletModifier _ENewTag);
	//Set Variables form outside
	UFUNCTION(BlueprintCallable)
	void SetComboAmount(float _fComboAmount);
	UFUNCTION(BlueprintCallable)
	void SetComboLevel(int _iNewComboLevel);
	//Used as callback for TakeDamage
	UFUNCTION()
	void OnAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	//Used as a callback for OnMovementModeChanged
	UFUNCTION()
	void OnStartFalling(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

	const UHealthComponentCpp* GetHealthComponent();
	bool GetIsDashing() const {return m_bIsDashing;}
	
	// Sets default values for this character's properties
	APlayerCharacter();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void PlayStepSound();
	

	//Delegate called when an action has been performed. Use for recording
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerAction, EPlayerActionType, eAction);

	UPROPERTY(BlueprintAssignable)
	FOnPlayerAction m_OnPlayerAction;
};
