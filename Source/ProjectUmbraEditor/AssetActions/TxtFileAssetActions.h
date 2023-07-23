#pragma once

#include "AssetTypeActions_Base.h"
#include "EditorStyleSet.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ContentBrowserModule.h"
#include "Framework/Commands/UIAction.h"
#include "IContentBrowserSingleton.h"
#include "AssetToolsModule.h"
#include "Automatinator/TxtFileAsset.h"

class  FTxtFileAssetActions : public FAssetTypeActions_Base {

	//GENERATED_BODY()

public:

	UClass* GetSupportedClass() const override { return UTxtFileAsset::StaticClass(); }
	FColor GetTypeColor() const override { return FColor::Yellow; }
	uint32 GetCategories()  override { return EAssetTypeCategories::Misc; }
	FText GetName() const override { return FText::FromString(TEXT("Txt File Asset")); }
	FText GetAssetDescription(const FAssetData& AssetData) const override 
	{
		return FText::FromString(AssetData.AssetName.ToString() + TEXT("Asset for txt file manipulation"));
	}

	//void InvertConversationActionClicked()
	//{
	//	if (FModuleManager::Get().IsModuleLoaded("ContentBrowser")) {
	//		FContentBrowserModule& ContentModule = FModuleManager::GetModuleChecked<FContentBrowserModule>("ContentBrowser");
	//		IContentBrowserSingleton& Browser = ContentModule.Get();
	//		TArray<FAssetData> Assets;
	//		Browser.GetSelectedAssets(Assets);
	//		for (FAssetData& Asset : Assets) {
	//			if (Asset.GetClass() == UConversationAsset::StaticClass()) {
	//				Cast<UConversationAsset>(Asset.GetAsset())->ReverseLines();
	//			}
	//		}

	//	}
	//}
	//bool HasActions(const TArray<UObject*>& InObjects) const override 
	//{
	//	return true;
	//}
	//void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override
	//{
	//	MenuBuilder.AddMenuEntry(
	//	FText::FromString(TEXT("Invertir conversacion")),
	//	FText::FromString(TEXT("Invierte el orden de lineas de una conversacion")),
	//		FSlateIcon(FEditorStyle::GetStyleSetName(),"LevelEditor.ViewOptions"),
	//		FUIAction(FExecuteAction::CreateRaw(this,&FConversationAssetActions::InvertConversationActionClicked))
	//	);
	//}

};