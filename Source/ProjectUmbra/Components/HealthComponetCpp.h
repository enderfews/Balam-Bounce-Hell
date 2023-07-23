// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "HealthComponetCpp.generated.h"

class UPlayerHUD;
class UDamageType;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTUMBRA_API UHealthComponentCpp : public UActorComponent
{
	GENERATED_BODY()
private:
	float m_fTimeToResetImmunityTime;
	bool m_bImmune = false;
	float m_fTimeToChangeColor;


	void UpdateTimers(float _fDeltaTime);
public:	
	float m_fHealth = 3.f;
	// Sets default values for this component's properties
	UHealthComponentCpp();
	//Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Max Health")
	float m_fMaxHealth = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "God Mode?")
	bool m_bGodMode = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Immunity Time")
	float m_fImmunityTime = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX", DisplayName = "Damaged Color Duration")
	float m_fDamagedColorDuration = 1.f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	void TakeDamage(float _Damage,bool _bFromDeathzone=false);
	UFUNCTION(BlueprintCallable)
	void HealDamage(float _HealAmount);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetHealth(float _fHealth);
	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	bool GetImmune() const;

	UFUNCTION(BlueprintCallable)
		void MakeInmmune();
	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnDeath );
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnImmunityEnd );
	UPROPERTY(BlueprintAssignable)
	FOnImmunityEnd OnImmunityEnd;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnDamaged, float, _fCurrentHealth );
	UPROPERTY(BlueprintAssignable)
	FOnDamaged OnDamaged;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnDamagedEnd );
	UPROPERTY(BlueprintAssignable)
	FOnDamagedEnd OnDamagedEnd;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnHealed, float, _fCurrentHealth );
	UPROPERTY(BlueprintAssignable)
	FOnHealed OnHealed;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnHealthChanged, float, _fCurrentHealth );
    UPROPERTY(BlueprintAssignable)
    FOnHealthChanged OnHealthChanged;
};
