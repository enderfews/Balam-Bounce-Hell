#include "BulletLUT.h"

void UBulletLUT::GetBulletData(uint8 _iTags, FBulletData& _FBulletDataToCopyInto) const
{
	const FBulletData FBulletDataSelected = BulletData[_iTags];
	_FBulletDataToCopyInto.m_fDamage = FBulletDataSelected.m_fDamage;
	_FBulletDataToCopyInto.m_bCanStun = FBulletDataSelected.m_bCanStun;
	_FBulletDataToCopyInto.m_bCanTrack = FBulletDataSelected.m_bCanTrack;
	_FBulletDataToCopyInto.m_iNumBounces = FBulletDataSelected.m_iNumBounces;
	_FBulletDataToCopyInto.m_iNumPierces = FBulletDataSelected.m_iNumPierces;
	_FBulletDataToCopyInto.m_fAreaOfEffect = FBulletDataSelected.m_fAreaOfEffect;
	_FBulletDataToCopyInto.m_UEnemyNSystem = FBulletDataSelected.m_UEnemyNSystem;
	_FBulletDataToCopyInto.m_UPlayerNSystem = FBulletDataSelected.m_UPlayerNSystem;
}
