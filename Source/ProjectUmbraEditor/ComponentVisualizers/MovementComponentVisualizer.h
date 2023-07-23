#pragma once

#include "ComponentVisualizer.h"

class UObjectMovementComponent;

// /**Base class for clickable targeting editing proxies*/
// struct HEndLocationVisProxy : public HComponentVisProxy
// {
// 	DECLARE_HIT_PROXY();
//
// 	HEndLocationVisProxy (const UActorComponent* InComponent)
// 	: HComponentVisProxy(InComponent, HPP_Wireframe)
// 	{}
// };

class PROJECTUMBRAEDITOR_API FMovementComponentVisualizer : public FComponentVisualizer
{
public:
	
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	// virtual bool VisProxyHandleClick(FEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click) override;
	// virtual bool GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const override;
	// virtual bool HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale) override;

	// UObjectMovementComponent* GetEditedMovementComponent() const;
	// UObjectMovementComponent* SelectedComponent;
	// bool m_bIsEditingEndLocation = false;
};
