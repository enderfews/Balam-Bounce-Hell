#include "BulletLUTActions.h"

#include "ProjectUmbra/CustomDataType/BulletLUT.h"

FBulletLUTActions::FBulletLUTActions(EAssetTypeCategories::Type _AssetCategory)
	: m_EAssetCategory(_AssetCategory)
{
}

FText FBulletLUTActions::GetName() const
{
	return FText::FromString(FString(TEXT("Bullet LUT")));
}

FColor FBulletLUTActions::GetTypeColor() const
{
	return FColor(125, 214, 47);
}

UClass* FBulletLUTActions::GetSupportedClass() const
{
	return UBulletLUT::StaticClass();
}

uint32 FBulletLUTActions::GetCategories()
{
	return m_EAssetCategory;	
}
