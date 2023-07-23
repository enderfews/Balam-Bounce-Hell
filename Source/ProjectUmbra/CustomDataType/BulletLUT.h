#pragma once

#include "BulletLUT.generated.h"

class UNiagaraSystem;

/**
 * ENUM with the types of projectiles.
 */
UENUM(BlueprintType)
enum class EBulletModifier : uint8
{
	Normal = 1 << 0,
	Explosive = 1 << 1,
	Bouncy = 1 << 2,
	Electric = 1 << 3,
	Freezing = 1 << 4,
	None = 0
};

USTRUCT(BlueprintType)
struct FBulletData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Damage"))
	float m_fDamage = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Can Track"))
	bool m_bCanTrack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Can Stun"))
	bool m_bCanStun = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Num Bounces"))
	int32 m_iNumBounces = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Num Pierces"))
	int32 m_iNumPierces= 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Area Of Effect"))
	float m_fAreaOfEffect = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Enemy owner VFX"))
	UNiagaraSystem* m_UEnemyNSystem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Player owner VFX"))
	UNiagaraSystem* m_UPlayerNSystem = nullptr;
};

UCLASS(BlueprintType)
class PROJECTUMBRA_API UBulletLUT : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<uint8,FBulletData> BulletData;

	UFUNCTION(BlueprintCallable)
	void GetBulletData(uint8 _iTags, FBulletData& _FBulletDataToCopyInto) const;
};