#pragma once

#include "AssetTypeActions_Base.h"

class FBulletLUTActions : public FAssetTypeActions_Base
{
public:
	FBulletLUTActions(EAssetTypeCategories::Type _AssetCategory);
	
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	
private:
	EAssetTypeCategories::Type m_EAssetCategory;
};
