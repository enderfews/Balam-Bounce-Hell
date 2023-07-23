#pragma once
#include "IDetailCustomization.h"

class BulletLUT;

class FBulletLUTDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	void RefreshDetails(bool Expanded, IDetailLayoutBuilder& DetailLayoutBuilder);
};
