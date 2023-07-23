#include "MovementComponentVisualizer.h"

#include "SceneManagement.h"
#include "ProjectUmbra/Components/ObjectMovementComponent.h"

// IMPLEMENT_HIT_PROXY(HEndLocationVisProxy, HComponentVisProxy);

void FMovementComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (const UObjectMovementComponent* UMovementComponent = Cast<UObjectMovementComponent>(Component))
	{
		//get colors for selected and unselected targets
		//This is an editor only uproperty of our targeting component, that way we can change the colors if we can't see them against the background
		const FVector VEndLocation = Component->GetOwner()->GetActorLocation() + UMovementComponent->m_VEndLocation;
		const FVector VStartLocation = Component->GetOwner()->GetActorLocation();
		
		const FLinearColor Color = FLinearColor::White;
	
		//Set our hit proxy
		// PDI->SetHitProxy(new HEndLocationVisProxy(Component));
		PDI->DrawLine(VStartLocation, VEndLocation, Color, SDPG_Foreground);
		PDI->DrawPoint(VEndLocation, Color, 20.f, SDPG_Foreground);
		// PDI->SetHitProxy(nullptr);
	}
}

// bool FMovementComponentVisualizer::VisProxyHandleClick(FEditorViewportClient* InViewportClient,
// 	HComponentVisProxy* VisProxy, const FViewportClick& Click)
// {
// 	bool bEditing = false;
//
// 	if (VisProxy && VisProxy->Component.IsValid())
// 	{
// 		m_bIsEditingEndLocation = bEditing = true;
// 	}
//
// 	return bEditing;
// }
//
// bool FMovementComponentVisualizer::GetWidgetLocation(const FEditorViewportClient* ViewportClient,
// 	FVector& OutLocation) const
// {
// 	const UActorComponent* AComponent = GetEditedComponent();
// 	const UObjectMovementComponent* Component = Cast<UObjectMovementComponent>(AComponent);
// 	if (m_bIsEditingEndLocation && Component)
// 	{
// 		OutLocation = Component->GetOwner()->GetActorLocation() + Component->m_VEndLocation;
//         
// 		return true;
// 	}
//
// 	return false;
// }
//
// bool FMovementComponentVisualizer::HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport,
// 	FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale)
// {
// 	bool bHandled = false;
//
// 	if (IsValid(GetEditedMovementComponent()) && m_bIsEditingEndLocation)
// 	{
// 		GetEditedMovementComponent()->m_VEndLocation += DeltaTranslate;
// 		bHandled = true;
// 	}
//
// 	return bHandled;
// }
//
// UObjectMovementComponent* FMovementComponentVisualizer::GetEditedMovementComponent() const
// {
// 	return Cast<UObjectMovementComponent>(GetEditedComponent());
// }
